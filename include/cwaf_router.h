#ifndef cwaf_router_h_included
#define cwaf_router_h_included

#include <libobject/array.h>
#include "cwaf_callback.h"

array_t * cwaf_router_new(const char *method, const char *regex, cwaf_cb callback, ...);

#endif /* ! cwaf_router_h_included */

