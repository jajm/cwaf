#ifndef cwaf_h_included
#define cwaf_h_included

#include <libobject/array.h>
#include <libobject/hash.h>

void cwaf_init(array_t *router);
hash_t * cwaf_parse_query_string(void);

#endif /* ! cwaf_h_included */
