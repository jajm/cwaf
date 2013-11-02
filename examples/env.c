#include <stdio.h>
#include <libobject/json.h>
#include "cwaf_router.h"
#include "cwaf_env.h"
#include "cwaf.h"

array_t * print_env_json()
{
	char *body;
	array_t *response;

	hash_t *env = cwaf_env_get();
	body = object_to_json(env, OBJECT_TO_JSON_VERY_PRETTY);
	cwaf_env_finalize();

	response = array(
		string("200 OK"),
		array(string("Content-type"), string("application/json; charset=utf-8")),
		array(string(body))
	);

	free(body);

	return response;
}

int main()
{
	array_t *router;

	router = cwaf_router_new(
		"", "", &print_env_json,
		NULL, NULL, NULL
	);
	cwaf_init(router);
	object_free(router);

	return 0;
}

