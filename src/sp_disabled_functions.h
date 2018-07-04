#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

extern zend_write_func_t zend_write_default;

int hook_disabled_functions();
int hook_echo(const char*, size_t);
bool should_disable(zend_execute_data *, const char *, const char *,
                    const char *);
bool should_drop_on_ret(zval *, const zend_execute_data *const);

#endif /* __SP_DISABLE_FUNCTIONS_H */
