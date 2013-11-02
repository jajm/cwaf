#include <stdio.h>
#include <libobject/json.h>
#include "cwaf_router.h"
#include "cwaf_env.h"
#include "cwaf.h"

array_t * print_hello_world()
{
	array_t *response;

	response = array(
		string("200 OK"),
		array(string("Content-type"), string("text/html; charset=utf-8")),
		array(string("<html><body><h1>Hello, world!</h1></body></html>"))
	);

	return response;
}

array_t * print_hello(array_t *args)
{
	array_t *response;
	string_t *body;

	body = string("<html><body><h1>Hello, ");
	string_scat(body, array_get(args, 0));
	string_cat(body, " ");
	if (array_size(args) > 1) {
		string_scat(body, array_get(args, 1));
	}
	string_cat(body, "</h1></body></html>");

	response = array(
		string("200 OK"),
		array(string("Content-type"), string("text/html; charset=utf-8")),
		array(body)
	);

	return response;
}

int main()
{
	array_t *router;

	router = cwaf_router_new(
		"", "^/([^/]+)/?([^/]+)?", &print_hello,
		"", "", &print_hello_world,
		NULL, NULL, NULL
	);
	cwaf_init(router);
	object_free(router);

	return 0;
}

