#define _POSIX_C_SOURCE 200809L // for strndup
#include <stdlib.h>
#include <string.h>
#include <fcgi_stdio.h>
#include <regex.h>
#include <libobject/array.h>
#include <libobject/hash.h>
#include <libobject/string.h>
#include <libobject/foreach.h>
#include "cwaf_env.h"
#include "cwaf_callback.h"
#include "cwaf_params.h"
#include "cwaf.h"

static array_t * response_error(const char *status, const char *message)
{
	array_t *response;

	response = array(
		string(status),
		array(string("Content-type"), string("text/html; charset=utf-8")),
		array(string("<html><body>", message, "</body></html>"))
	);

	return response;
}

static array_t * response_error_500(const char *message)
{
	return response_error("500 Internal Server Error", message);
}

static array_t * response_error_404(const char *message)
{
	return response_error("404 Not Found", message);
}

int cwaf_puts(const char *s)
{
	if (s == NULL)
		return -1;

	return fputs(s, stdout);
}

int cwaf_putc(char c)
{
	return fputc(c, stdout);
}

void response_render(array_t *response)
{
	string_t *status = array_get(response, 0);
	array_t *header = array_get(response, 1);
	array_t *body = array_get(response, 2);
	char *server_protocol;

	server_protocol = getenv("SERVER_PROTOCOL");
	cwaf_puts(server_protocol);
	cwaf_putc(' ');
	cwaf_puts(string_to_c_str(status));
	unsigned int size = array_size(header);
	for (unsigned int i = 1; i < size; i += 2) {
		string_t *key = array_get(header, i-1);
		string_t *value = array_get(header, i);
		cwaf_puts(string_to_c_str(key));
		cwaf_puts(": ");
		cwaf_puts(string_to_c_str(value));
		cwaf_putc('\n');
	}
	cwaf_putc('\n');
	foreach(body, s) {
		cwaf_puts(string_to_c_str(s));
	}
	cwaf_putc('\n');
}

array_t * args_build_from_matches(char *path_info, size_t nmatch, regmatch_t *pmatch)
{
	array_t *args = array();

	for (unsigned int i = 1; i <= nmatch && pmatch[i].rm_so != -1; i++) {
		char c = path_info[pmatch[i].rm_eo];
		path_info[pmatch[i].rm_eo] = '\0';
		array_push(args, string(path_info + pmatch[i].rm_so));
		path_info[pmatch[i].rm_eo] = c;
	}

	return args;
}

void cwaf_init(array_t *router)
{
	while (FCGI_Accept() >= 0) {
		cwaf_cb callback = NULL;
		regex_t preg;
		size_t nmatch = 10;
		regmatch_t pmatch[10];
		char *path_info;
		char *server_protocol;
		array_t *response;
		iterator_t *it;
		char buffer[80];
		int rc;

		path_info = getenv("PATH_INFO");
		if (path_info == NULL) {
			response = response_error_500("PATH_INFO is not defined");
			response_render(response);
			continue;
		}

		server_protocol = getenv("SERVER_PROTOCOL");
		if (server_protocol == NULL) {
			response = response_error_500("SERVER_PROTOCOL is not defined");
			response_render(response);
			return;
		}

		it = object_iterator_new(router);
		while (!iterator_step(it)) {
			array_t *router_entry = iterator_get(it);
			const char *method = string_to_c_str(array_get(router_entry, 0));
			const char *regex;

			if (method[0] != '\0' && strcmp(method, getenv("REQUEST_METHOD"))) {
				continue;
			}

			regex = string_to_c_str(array_get(router_entry, 1));

			if ((rc = regcomp(&preg, regex, REG_EXTENDED))) {
				regerror(rc, &preg, buffer, 80);
				fprintf(stderr, "regex compilation failed: %s %s\n", buffer, regex);
				continue;
			}

			if (!regexec(&preg, path_info, nmatch, pmatch, 0)) {
				callback = cwaf_callback_get(array_get(router_entry, 2));
				break;
			}
		}
		regfree(&preg);
		iterator_free(it);

		if (callback != NULL) {
			array_t *args = args_build_from_matches(path_info,
				nmatch, pmatch);

			response = callback(args);
			response_render(response);
			object_free(args);
			object_free(response);
		} else {
			response = response_error_404("");
			response_render(response);
			object_free(response);
		}

		cwaf_params_finalize();
	}
}

