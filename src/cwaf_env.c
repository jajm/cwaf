#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libobject/hash.h>
#include <libobject/string.h>
#include <libobject/dumper.h>
#include <libobject/json.h>
#include "cwaf_env.h"

static hash_t *env = NULL;
extern char **environ;

int cwaf_env_initialize(void)
{
	char **e, *ptr;

	if (env != NULL) {
		cwaf_env_finalize();
	}

	env = hash_new();

	e = environ;
	while (*e != NULL) {
		ptr = strchr(*e, '=');
		if (ptr != NULL) {
			*ptr = '\0';
			hash_set(env, *e, string_new(ptr+1));
			*ptr = '=';
		}
		e++;
	}

	return 0;
}

hash_t * cwaf_env_get(void)
{
	if (env == NULL) {
		cwaf_env_initialize();
	}

	return env;
}

void cwaf_env_finalize(void)
{
	hash_free(env);
	env = NULL;
}
