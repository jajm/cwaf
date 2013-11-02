#ifndef cwaf_callback_h_included
#define cwaf_callback_h_included

#include <libobject/object.h>
#include <libobject/array.h>
#include <libobject/hash.h>

typedef array_t *(*cwaf_cb)(array_t *args);

typedef object_t cwaf_callback_t;

cwaf_callback_t * cwaf_callback_new(cwaf_cb cb);
cwaf_cb cwaf_callback_get(cwaf_callback_t *callback);

#endif /* ! cwaf_callback_h_included */

