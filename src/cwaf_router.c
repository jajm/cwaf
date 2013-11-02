#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <libobject/string.h>
#include <libobject/array.h>
#include <libobject/json.h>
#include "cwaf_callback.h"

array_t * cwaf_router_new(const char *method, const char *regex, cwaf_cb callback, ...)
{
	array_t *router, *router_entry;
	va_list va_ptr;

	router = array();
	va_start(va_ptr, callback);
	while (method != NULL && regex != NULL && callback != NULL) {
		router_entry = array(
			string_new(method),
			string_new(regex),
			cwaf_callback_new(callback)
		);
		array_push(router, router_entry);

		method = va_arg(va_ptr, const char *);
		regex = va_arg(va_ptr, const char *);
		callback = va_arg(va_ptr, cwaf_cb);
	}
	va_end(va_ptr);

	return router;
}
