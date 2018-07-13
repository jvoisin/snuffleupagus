#ifndef SP_CONFIG_UTILS
#define SP_CONFIG_UTILS

int parse_keywords(sp_config_functions *, char *);
zend_string *get_param(size_t *, char *restrict, sp_type, const char *restrict);
int array_to_list(char **, sp_list_node **);
sp_list_node *parse_functions_list(char *value);

#endif /* SP_CONFIG_UTILS */
