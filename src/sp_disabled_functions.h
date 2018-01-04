#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

int hook_disabled_functions();
bool should_disable(zend_execute_data *, const char *, const char *,
                    const char *);
bool should_drop_on_ret(zval *, const zend_execute_data *const);
char* get_complete_function_path(zend_execute_data const* const execute_data);

#endif /* __SP_DISABLE_FUNCTIONS_H */
