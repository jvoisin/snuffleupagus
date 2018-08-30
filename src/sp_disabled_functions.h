#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

extern zend_write_func_t zend_write_default;

int hook_disabled_functions();
int hook_echo(const char *, size_t);
bool should_disable(zend_execute_data *, const char *, const zend_string *,
                    const char *, const sp_list_node *, const zend_string *);
bool should_disable_ht(zend_execute_data *, const char *, const zend_string *,
                       const char *, const sp_list_node *, const HashTable *);
bool should_drop_on_ret_ht(const zval *, const char *, const sp_list_node *config,
                           const HashTable *, zend_execute_data *);
bool should_drop_on_ret(const zval *, const sp_list_node *config, const char *,
                        zend_execute_data *);
char *get_complete_function_path(zend_execute_data const *const);

#endif /* __SP_DISABLE_FUNCTIONS_H */
