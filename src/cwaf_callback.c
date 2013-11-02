#include <stdlib.h>
#include <libobject/object.h>
#include "cwaf_callback.h"

static const char cwaf_callback_type[] = "cwaf_callback";

cwaf_callback_t * cwaf_callback_new(cwaf_cb cb)
{
	cwaf_callback_t *callback;

	callback = object_new(cwaf_callback_type, cb);

	return callback;
}

cwaf_cb cwaf_callback_get(cwaf_callback_t *callback)
{
	cwaf_cb cb = NULL;

	if (object_isa(callback, cwaf_callback_type)) {
		cb = object_value(callback);
	}

	return cb;
}
