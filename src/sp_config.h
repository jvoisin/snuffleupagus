#ifndef SP_CONFIG_H
#define SP_CONFIG_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef enum {
  SP_PHP_TYPE_UNDEF = IS_UNDEF,
  SP_PHP_TYPE_NULL = IS_NULL,
  SP_PHP_TYPE_FALSE = IS_FALSE,
  SP_PHP_TYPE_TRUE = IS_TRUE,
  SP_PHP_TYPE_LONG = IS_LONG,
  SP_PHP_TYPE_DOUBLE = IS_DOUBLE,
  SP_PHP_TYPE_STRING = IS_STRING,
  SP_PHP_TYPE_ARRAY = IS_ARRAY,
  SP_PHP_TYPE_OBJECT = IS_OBJECT,
  SP_PHP_TYPE_RESOURCE = IS_RESOURCE,
  SP_PHP_TYPE_REFERENCE = IS_REFERENCE
} sp_php_type;

typedef enum { SP_ZEND = 0, SP_SYSLOG = 1 } sp_log_media;

typedef enum { SP_UNSET = 0, SP_READONLY = 1, SP_READWRITE = -1 } sp_ini_permission;

typedef struct {
  int ip_version;
  union {
    struct in_addr ipv4;
    struct in6_addr ipv6;
  } ip;
  uint8_t mask;
} sp_cidr;

typedef struct {
  bool enable;
  bool simulation;
  bool extended_checks;
  zend_string *dump;
  zend_string *textual_representation;
} sp_config_readonly_exec;

typedef struct {
  bool enable;
} sp_config_global_strict;

typedef struct {
  bool enable;
} sp_config_random;

typedef struct {
  bool enable;
} sp_config_sloppy;

typedef struct {
  bool enable;
} sp_config_auto_cookie_secure;

typedef struct {
  bool enable;
} sp_config_xxe_protection;

typedef struct {
  enum samesite_type { strict = 1, lax = 2 } samesite;
  bool encrypt;
  zend_string *name;
  sp_regexp *name_r;
  bool simulation;
} sp_cookie;

typedef struct {
  sp_list_node *whitelist;
  sp_list_node *php_stream_allowlist;
  bool enabled;
  size_t num_wrapper;  // Used to verify if wrappers were added.
} sp_config_wrapper;

typedef struct {
  bool encrypt;
  bool simulation;
  u_long sid_min_length;
  u_long sid_max_length;
} sp_config_session;

typedef struct {
  bool enable;
  zend_string *textual_representation;
} sp_config_unserialize_noclass;

typedef struct {
  bool enable;
  bool simulation;
  zend_string *dump;
  zend_string *textual_representation;
} sp_config_unserialize;

typedef struct {
  zend_string *textual_representation;

  zend_string *filename;
  sp_regexp *r_filename;

  zend_string *function;
  sp_regexp *r_function;
  sp_list_node *functions_list;

  zend_string *hash;
  int simulation;

  sp_tree *param;
  sp_regexp *r_param;
  sp_php_type param_type;
  int pos;
  unsigned int line;

  sp_regexp *r_ret;
  zend_string *ret;
  sp_php_type ret_type;

  sp_regexp *r_value;
  zend_string *value;

  sp_regexp *r_key;
  zend_string *key;

  zend_string *dump;
  zend_string *alias;
  bool param_is_array;
  bool var_is_array;
  // sp_list_node *param_array_keys;
  // sp_list_node *var_array_keys;

  bool allow;

  sp_tree *var;

  sp_cidr *cidr;
} sp_disabled_function;

typedef struct {
  sp_list_node *blacklist;
  sp_list_node *whitelist;
  bool simulation;
  zend_string *dump;
  zend_string *textual_representation;
} sp_config_eval;

typedef struct {
  sp_list_node *disabled_functions;  // list of sp_disabled_function
} sp_config_disabled_functions;

typedef struct {
  sp_list_node *cookies;  // list of sp_cookie for regexp/names
} sp_config_cookie;

typedef struct {
  zend_string *script;
  bool simulation;
  bool enable;
} sp_config_upload_validation;

typedef struct {
  zend_string *key;
  sp_ini_permission access;
  zend_string *min;
  zend_string *max;
  sp_regexp *regexp;
  zend_string *msg;
  zend_string *set;
  bool allow_null;
  bool simulation;
  bool drop;
  PHP_INI_MH((*orig_onmodify));
} sp_ini_entry;

typedef struct {
  bool enable;
  bool simulation;
  bool policy_readonly;
  bool policy_silent_ro;
  bool policy_silent_fail;
  bool policy_drop;
  HashTable *entries;  // ht of sp_ini_entry
} sp_config_ini;

#define SP_PARSE_FN_(fname, kwvar) int fname(char const *const token, sp_parsed_keyword *kwvar, void *retval)
#define SP_PARSE_FN(fname) SP_PARSE_FN_(fname, parsed_rule)
#define SP_PARSEKW_FN(fname) SP_PARSE_FN_(fname, kw)

typedef struct {
  SP_PARSE_FN((*func));
  const char *token;
  void *retval;
} sp_config_keyword;

#define SP_PARSER_SUCCESS 0
#define SP_PARSER_ERROR -1
#define SP_PARSER_STOP 1

// #define SP_TOKEN_BASE "sp"

#define SP_TOKEN_AUTO_COOKIE_SECURE "auto_cookie_secure"
#define SP_TOKEN_COOKIE_ENCRYPTION "cookie"
#define SP_TOKEN_SESSION_ENCRYPTION "session"
#define SP_TOKEN_DISABLE_FUNC "disable_function"
#define SP_TOKEN_GLOBAL "global"
#define SP_TOKEN_GLOBAL_STRICT "global_strict"
#define SP_TOKEN_HARDEN_RANDOM "harden_random"
#define SP_TOKEN_READONLY_EXEC "readonly_exec"
#define SP_TOKEN_UNSERIALIZE_HMAC "unserialize_hmac"
#define SP_TOKEN_UNSERIALIZE_NOCLASS "unserialize_noclass"
#define SP_TOKEN_UPLOAD_VALIDATION "upload_validation"
#define SP_TOKEN_XXE_PROTECTION "xxe_protection"
#define SP_TOKEN_EVAL_BLACKLIST "eval_blacklist"
#define SP_TOKEN_EVAL_WHITELIST "eval_whitelist"
#define SP_TOKEN_SLOPPY_COMPARISON "sloppy_comparison"
#define SP_TOKEN_ALLOW_WRAPPERS "wrappers_whitelist"
#define SP_TOKEN_ALLOW_PHP_STREAMS "php_list"
#define SP_TOKEN_INI_PROTECTION "ini_protection"
#define SP_TOKEN_INI "ini"

// common tokens
#define SP_TOKEN_ENABLE "enable"
#define SP_TOKEN_DISABLE "disable"
#define SP_TOKEN_SIMULATION "simulation"
#define SP_TOKEN_SIM "sim"
// #define SP_TOKEN_TRUE "1"
// #define SP_TOKEN_FALSE "0"
#define SP_TOKEN_DUMP "dump"
#define SP_TOKEN_ALIAS "alias"
#define SP_TOKEN_ALLOW "allow"
#define SP_TOKEN_DROP "drop"

// disable_function
#define SP_TOKEN_CIDR "cidr"
#define SP_TOKEN_FILENAME "filename"
#define SP_TOKEN_FILENAME_REGEXP "filename_r"
#define SP_TOKEN_FUNCTION "function"
#define SP_TOKEN_FUNCTION_REGEXP "function_r"
#define SP_TOKEN_HASH "hash"
#define SP_TOKEN_LOCAL_VAR "var"
#define SP_TOKEN_PARAM "param"
#define SP_TOKEN_PARAM_REGEXP "param_r"
#define SP_TOKEN_PARAM_TYPE "param_type"
#define SP_TOKEN_RET "ret"
#define SP_TOKEN_RET_REGEXP "ret_r"
#define SP_TOKEN_RET_TYPE "ret_type"
#define SP_TOKEN_VALUE "value"
#define SP_TOKEN_VALUE_REGEXP "value_r"
#define SP_TOKEN_KEY "key"
#define SP_TOKEN_KEY_REGEXP "key_r"
#define SP_TOKEN_VALUE_ARG_POS "pos"
#define SP_TOKEN_LINE_NUMBER "line"

// cookies encryption
#define SP_TOKEN_NAME "name"
#define SP_TOKEN_NAME_REGEXP "name_r"

// cookies samesite
#define SP_TOKEN_SAMESITE "samesite"
#define SP_TOKEN_ENCRYPT "encrypt"
#define SP_TOKEN_SAMESITE_LAX "Lax"
#define SP_TOKEN_SAMESITE_STRICT "Strict"

// Global configuration options
#define SP_TOKEN_ENCRYPTION_KEY "secret_key"
#define SP_TOKEN_ENV_VAR "cookie_env_var"
#define SP_TOKEN_LOG_MEDIA "log_media"
#define SP_TOKEN_LOG_MAX_LEN "log_max_len"
#define SP_TOKEN_MAX_EXECUTION_DEPTH "max_execution_depth"
#define SP_TOKEN_SERVER_ENCODE "server_encode"
#define SP_TOKEN_SERVER_STRIP "server_strip"
#define SP_TOKEN_SID_MIN_LENGTH "sid_min_length"
#define SP_TOKEN_SID_MAX_LENGTH "sid_max_length"
#define SP_TOKEN_SHOW_OLD_PHP_WARNING "show_old_php_warning"

// upload_validator
#define SP_TOKEN_UPLOAD_SCRIPT "script"

#define SP_TOKEN_LIST "list"

zend_result sp_process_rule(sp_parsed_keyword *parsed_rule, const sp_config_keyword *const config_keywords);

zend_result sp_parse_config(const char *const filename);

#define SP_PARSE_CHECK_ARG_EXISTS(value) \
if (!value) { \
  sp_log_err("config", "Missing argument to keyword '%s' - it should be '%s(\"...\")' on line %zu", token, token, kw->lineno); \
  return SP_PARSER_ERROR; \
}

#define SP_PARSE_ARG(value) \
  zend_string *value = sp_get_arg_string(kw); \
  SP_PARSE_CHECK_ARG_EXISTS(value);

SP_PARSEKW_FN(parse_str);
SP_PARSEKW_FN(parse_regexp);
SP_PARSEKW_FN(parse_empty);
SP_PARSEKW_FN(parse_int);
SP_PARSEKW_FN(parse_uint);
SP_PARSEKW_FN(parse_php_type);
SP_PARSEKW_FN(parse_cidr);
SP_PARSEKW_FN(parse_list);

// cleanup
void sp_free_disabled_function(void *data);
void sp_free_cookie(void *data);
void sp_free_zstr(void *data);
void sp_free_ini_entry(void *data);

#endif /* SP_CONFIG_H */
