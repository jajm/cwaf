#define _POSIX_C_SOURCE 200809L // for strndup
#include <regex.h>
#include <stdlib.h>
#include <fcgi_stdio.h>
#include <string.h>
#include <libobject/hash.h>
#include <libobject/json.h>

static hash_t *cwaf_params = NULL;

void cwaf_params_add(hash_t *params, const char *name, object_t *value)
{
	object_t *param;

	param = hash_get(params, name);
	if (object_is_array(param)) {
		array_push(param, value);
	} else if (object_isset(param)) {
		object_t *copy = object_copy(param);
		array_t *a = array(copy, value);
		hash_set(params, name, a);
	} else {
		hash_set(params, name, value);
	}
}

hash_t * cwaf_parse_data_string(const char *data)
{
	hash_t *params;
	const char *start, *end;
	string_t *name, *value;
	int done = 0;

	params = hash();

	start = data;
	while (!done) {
		name = NULL;
		value = NULL;
		end = strpbrk(start, "=&;");
		if (end != NULL) {
			name = string_new_nocopy(strndup(start, end - start));

			start = end + 1;

			if (*end == '=') {
				end = strpbrk(start, "&;");
				if (end != NULL) {
					value = string_new_nocopy(strndup(start, end - start));
					start = end + 1;
				} else {
					value = string(start);
					done = 1;
				}
			}
		} else {
			name = string(start);
			done = 1;
		}

		cwaf_params_add(params, string_to_c_str(name), value);
		object_free(name);
	}

	return params;
}

hash_t * cwaf_parse_query_string(void)
{
	return cwaf_parse_data_string(getenv("QUERY_STRING"));
}

hash_t * cwaf_parse_multipart_headers(const char **ptr)
{
	hash_t *headers = NULL;
	const char *tmp;
	int i;

	headers = hash();
	while (**ptr != '\r' && *((*ptr)+1) != '\n') {
		char *fieldname;
		string_t *fieldbody;

		tmp = strchr(*ptr, ':');
		fieldname = malloc(tmp - (*ptr) + 1);
		i = 0;
		while (*ptr != tmp) {
			fieldname[i++] = **ptr;
			(*ptr)++;
		}
		fieldname[i] = '\0';

		// Go to field body.
		(*ptr) += 2;

		tmp = strstr(*ptr, "\r\n");
		fieldbody = string_new_from_substring(*ptr, 0, tmp - (*ptr));

		hash_set(headers, fieldname, fieldbody);
		free(fieldname);

		// Go to next header.
		(*ptr) = tmp + 2;
	}

	// Go to body
	(*ptr) += 2;

	return headers;
}

hash_t * cwaf_parse_multipart_header_body(const char *body)
{
	hash_t *data = hash();
	const char *start, *end;
	regex_t preg;
	size_t nmatch = 3;
	regmatch_t pmatch[3];
	const char *regex = "([a-zA-Z]+)=\"([^\"]*)\"";
	int rc;
	char *name, *value;
	char buffer[512];

	start = body;
	end = strpbrk(start, ";");
	if (end) {
		hash_set(data, "", string_new_from_substring(start, 0, end - start));
	} else {
		hash_set(data, "", string_new(start));
	}

	if ((rc = regcomp(&preg, regex, REG_EXTENDED))) {
		regerror(rc, &preg, buffer, 512);
		fprintf(stderr, "regex compilation failed: %s %s\n", buffer, regex);
		return NULL;
	}

	while (end != NULL) {
		start = end + 1;

		if (!regexec(&preg, start, nmatch, pmatch, 0)) {
			if (pmatch[1].rm_so != -1) {
				name = strndup(start + pmatch[1].rm_so,
					pmatch[1].rm_eo - pmatch[1].rm_so);
				value = strndup(start + pmatch[2].rm_so,
					pmatch[2].rm_eo - pmatch[2].rm_so);
				hash_set(data, name, string_new_nocopy(value));
				free(name);
			}
		}
		end = strpbrk(start, ";");
	}
	regfree(&preg);

	return data;
}

hash_t * cwaf_parse_multipart_input(string_t *input_s)
{
	hash_t *params = NULL;
	const char *content_type;
	char *boundary = NULL;
	size_t boundary_length = 0;
	regex_t preg;
	size_t nmatch = 2;
	regmatch_t pmatch[2];
	const char *regex = "boundary=([-0-9a-zA-Z]+)";
	char buffer[512];
	int rc;

	content_type = getenv("CONTENT_TYPE");

	if ((rc = regcomp(&preg, regex, REG_EXTENDED))) {
		regerror(rc, &preg, buffer, 512);
		fprintf(stderr, "regex compilation failed: %s %s\n", buffer, regex);
		return NULL;
	}

	if (!regexec(&preg, content_type, nmatch, pmatch, 0)) {
		if (pmatch[1].rm_so != -1) {
			boundary_length = pmatch[1].rm_eo - pmatch[1].rm_so;
			boundary = strndup(content_type + pmatch[1].rm_so,
				boundary_length);
		}
	}
	regfree(&preg);

	if (boundary != NULL) {
		params = hash();
		const char *input_ptr, *input;
		const char *tmp;
		size_t input_size;
		char *delimiter, *enddelimiter;

		input_ptr = input = string_to_c_str(input_s);
		input_size = string_length(input_s);

		delimiter = calloc(boundary_length + 5, 1);
		strcat(delimiter, "\r\n--");
		strcat(delimiter + 4, boundary);

		enddelimiter = calloc(boundary_length + 7, 1);
		strcat(enddelimiter, delimiter);
		strcat(enddelimiter + 4 + boundary_length, "--");

		input_ptr = strstr(input_ptr, delimiter + 2);
		while (strncmp(input_ptr, enddelimiter, boundary_length + 6)
		       && (unsigned)(input_ptr - input) < input_size)
		{
			hash_t *headers;
			hash_t *data;
			const char *name;
			string_t *filename;
			string_t *body;
			string_t *content_disposition;

			// Parse headers to find parameter name
			input_ptr = strstr(input_ptr + 1, "\r\n") + 2;
			headers = cwaf_parse_multipart_headers(&input_ptr);
			content_disposition = hash_get(headers, "Content-Disposition");
			data = cwaf_parse_multipart_header_body(string_to_c_str(content_disposition));
			name = string_to_c_str(hash_get(data, "name"));

			// Retrieve parameter value
			tmp = strstr(input_ptr, delimiter);
			body = string_new_from_substring(input_ptr, 0, tmp - input_ptr);

			filename = hash_get(data, "filename");
			if (object_isset(filename)) {
				// It's a file.
				hash_t *content_type_data;
				string_t *content_type;

				content_type_data = cwaf_parse_multipart_header_body(string_to_c_str(hash_get(headers, "Content-Type")));
				content_type = hash_get(content_type_data, "");
				hash_t *file = hash(
					"filename", object_copy(filename),
					"type", object_copy(content_type),
					"content", body
				);
				cwaf_params_add(params, name, file);
				object_free(content_type_data);
			} else {
				cwaf_params_add(params, name, body);
			}

			object_free(data);
			object_free(headers);

			input_ptr = tmp;
		}

		free(delimiter);
		free(enddelimiter);
		free(boundary);
	}

	return params;
}

hash_t * cwaf_parse_plain_input(const char *data)
{
	hash_t *params;
	const char *start, *end;
	string_t *name, *value;

	params = hash();

	start = data;
	while (*start != '\0') {
		name = NULL;
		value = NULL;
		end = strchr(start, '=');
		if (end != NULL) {
			name = string_new_nocopy(strndup(start, end - start));

			start = end + 1;

			if (*end == '=') {
				end = strstr(start, "\r\n");
				if (end != NULL) {
					value = string_new_nocopy(strndup(start, end - start));
					start = end + 2;
				} else {
					value = string(start);
					start = strchr(start, '\0');
				}
			}

			cwaf_params_add(params, string_to_c_str(name), value);
			object_free(name);
		}

	}

	return params;
}

hash_t * cwaf_parse_input(void)
{
	string_t *input;
	char buffer[512];
	hash_t *params;
	const char *content_type;

	input = string("");
	while (!feof(stdin)) {
		size_t read = fread(buffer, 1, 511, stdin);
		buffer[read] = '\0';
		string_cat(input, buffer);
	}

	content_type = getenv("CONTENT_TYPE");
	if (!strncmp(content_type, "multipart/form-data;", 20)) {
		params = cwaf_parse_multipart_input(input);
	} else if (!strncmp(content_type, "text/plain", 10)) {
		params = cwaf_parse_plain_input(string_to_c_str(input));
	} else {
		params = cwaf_parse_data_string(string_to_c_str(input));
	}

	object_free(input);

	return params;
}

void  cwaf_params_initialize(void)
{
	if (!strcmp("GET", getenv("REQUEST_METHOD"))) {
		cwaf_params = cwaf_parse_query_string();
	} else {
		cwaf_params = cwaf_parse_input();
	}
}

const object_t * cwaf_params_get(const char *name)
{
	if (cwaf_params == NULL) {
		cwaf_params_initialize();
	}

	return hash_get(cwaf_params, name);
}

const hash_t * cwaf_params_get_all(void)
{
	if (cwaf_params == NULL) {
		cwaf_params_initialize();
	}

	return cwaf_params;
}

void cwaf_params_finalize(void)
{
	object_free(cwaf_params);
	cwaf_params = NULL;
}
