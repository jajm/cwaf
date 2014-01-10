#ifndef cwaf_params_h_included
#define cwaf_params_h_included

const object_t * cwaf_params_get(const char *name);
const hash_t * cwaf_params_get_all(void);
void cwaf_params_finalize(void);

#endif /* ! cwaf_params_h_included */
