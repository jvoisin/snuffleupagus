#ifndef SP_CONFIG_KEYWORDS_H
#define SP_CONFIG_KEYWORDS_H
#include "php_snuffleupagus.h"

SP_PARSE_FN(parse_enable);
SP_PARSE_FN(parse_global);
SP_PARSE_FN(parse_cookie);
SP_PARSE_FN(parse_unserialize);
SP_PARSE_FN(parse_unserialize_noclass);
SP_PARSE_FN(parse_readonly_exec);
SP_PARSE_FN(parse_disabled_functions);
SP_PARSE_FN(parse_upload_validation);
SP_PARSE_FN(parse_eval_filter_conf);
SP_PARSE_FN(parse_session);
SP_PARSE_FN(parse_wrapper_whitelist);
SP_PARSE_FN(parse_log_media);
SP_PARSE_FN(parse_ini_protection);
SP_PARSE_FN(parse_ini_entry);

#endif  // __SP_CONFIG_KEYWORDS_H
