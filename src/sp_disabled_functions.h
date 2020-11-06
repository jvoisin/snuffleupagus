#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

extern zend_write_func_t zend_write_default;

int hook_disabled_functions(void);
int hook_echo(const char *, size_t);
void should_disable_ht(zend_execute_data *, const char *, const zend_string *,
                       const char *, const sp_list_node *, const HashTable *);
void should_drop_on_ret_ht(const zval *, const char *,
                           const sp_list_node *config, const HashTable *,
                           zend_execute_data *);
char *get_complete_function_path(zend_execute_data const *const);

#endif /* __SP_DISABLE_FUNCTIONS_H */
