#ifndef SP_CONFIG_UTILS
#define SP_CONFIG_UTILS

int parse_keywords(sp_config_functions *, char *);
char *get_param(size_t *, char *restrict, sp_type, const char *restrict);
int array_to_list(char **, sp_node_t **);
sp_node_t *parse_functions_list(char *value);

#endif /* SP_CONFIG_UTILS */
