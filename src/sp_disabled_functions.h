#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

int hook_disabled_functions();
bool should_disable(zend_execute_data *, const char *, const zend_string *,
                    const char *, const sp_list_node *, const zend_string *);
bool should_disable_ht(zend_execute_data *, const char *, const zend_string *,
                    const char *, const sp_list_node *, const HashTable *);
//const sp_list_node* get_config_node(const char* builtin_name);
bool should_drop_on_ret_ht(zval *, const zend_execute_data *const, const sp_list_node* config, const HashTable *);
bool should_drop_on_ret(zval *, const sp_list_node* config, const char *);

#endif /* __SP_DISABLE_FUNCTIONS_H */
