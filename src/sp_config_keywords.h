#ifndef SP_CONFIG_KEYWORDS_H
#define SP_CONFIG_KEYWORDS_H
#include "php_snuffleupagus.h"

int parse_random(char *line);
int parse_disable_xxe(char *line);
int parse_auto_cookie_secure(char *line);
int parse_global_strict(char *line);
int parse_global(char *line);
int parse_cookie(char *line);
int parse_unserialize(char *line);
int parse_readonly_exec(char *line);
int parse_disabled_functions(char *line);
int parse_upload_validation(char *line);
int parse_eval_blacklist(char *line);
int parse_eval_whitelist(char *line);

#endif  // __SP_CONFIG_KEYWORDS_H
