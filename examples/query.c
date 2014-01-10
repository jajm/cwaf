#include <stdio.h>
#include <libobject/json.h>
#include "cwaf_router.h"
#include "cwaf_env.h"
#include "cwaf_params.h"
#include "cwaf.h"

array_t * print_query()
{
	array_t *response;
	string_t *body;
	const hash_t *params;

	params = cwaf_params_get_all();
	body = string(object_to_json(params, OBJECT_TO_JSON_VERY_PRETTY));

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
		"", "", &print_query,
		NULL, NULL, NULL
	);
	cwaf_init(router);
	object_free(router);

	return 0;
}

