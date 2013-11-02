#ifndef cwaf_env_h_included
#define cwaf_env_h_included

#include <libobject/hash.h>

int
cwaf_env_initialize(void);

hash_t *
cwaf_env_get(void);

void
cwaf_env_finalize(void);

#endif /* ! cwaf_env_h_included */

