#ifndef SP_CONFIG_H
#define SP_CONFIG_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

extern size_t sp_line_no;

typedef enum {
  SP_TYPE_STR = 0,
  SP_TYPE_REGEXP,
  SP_TYPE_INT,
  SP_TYPE_EMPTY
} sp_type;

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

typedef struct {
  int ip_version;
  union {
    struct in_addr ipv4;
    struct in6_addr ipv6;
  } ip;
  uint8_t mask;
} sp_cidr;

typedef struct {
  zend_string *encryption_key;
  zend_string *cookies_env_var;
} sp_config_global;

typedef struct {
  bool enable;
  bool simulation;
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
} sp_config_disable_xxe;

typedef struct {
  enum samesite_type { strict = 1, lax = 2 } samesite;
  bool encrypt;
  zend_string *name;
  sp_pcre *name_r;
  bool simulation;
} sp_cookie;

typedef struct {
  sp_list_node *whitelist;
  bool enabled;
  size_t num_wrapper;  // Used to verify if wrappers were added.
} sp_config_wrapper;

typedef struct {
  bool encrypt;
  bool simulation;
} sp_config_session;

typedef struct {
  bool enable;
  bool simulation;
  zend_string *dump;
  zend_string *textual_representation;
} sp_config_unserialize;

typedef struct {
  zend_string *textual_representation;

  zend_string *filename;
  sp_pcre *r_filename;

  zend_string *function;
  sp_pcre *r_function;
  sp_list_node *functions_list;

  zend_string *hash;
  int simulation;

  sp_tree *param;
  sp_pcre *r_param;
  sp_php_type param_type;
  int pos;
  unsigned int line;

  sp_pcre *r_ret;
  zend_string *ret;
  sp_php_type ret_type;

  sp_pcre *r_value;
  zend_string *value;

  sp_pcre *r_key;
  zend_string *key;

  zend_string *dump;
  zend_string *alias;
  bool param_is_array;
  bool var_is_array;
  sp_list_node *param_array_keys;
  sp_list_node *var_array_keys;

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
  sp_config_random *config_random;
  sp_config_sloppy *config_sloppy;
  sp_config_unserialize *config_unserialize;
  sp_config_readonly_exec *config_readonly_exec;
  sp_config_upload_validation *config_upload_validation;
  sp_config_cookie *config_cookie;
  sp_config_global *config_snuffleupagus;
  sp_config_auto_cookie_secure *config_auto_cookie_secure;
  sp_config_global_strict *config_global_strict;
  sp_config_disable_xxe *config_disable_xxe;
  sp_config_eval *config_eval;
  sp_config_wrapper *config_wrapper;
  sp_config_session *config_session;
  bool hook_execute;
  char log_media;

  HashTable *config_disabled_functions;
  HashTable *config_disabled_functions_hooked;
  HashTable *config_disabled_functions_ret;
  HashTable *config_disabled_functions_ret_hooked;
  sp_config_disabled_functions *config_disabled_functions_reg;
  sp_config_disabled_functions *config_disabled_functions_reg_ret;
} sp_config;

typedef struct {
  int (*func)(char *, char *, void *);
  char *token;
  void *retval;
} sp_config_functions;

typedef struct {
  int (*func)(char *);
  char *token;
} sp_config_tokens;

#define SP_TOKEN_BASE "sp"

#define SP_TOKEN_AUTO_COOKIE_SECURE ".auto_cookie_secure"
#define SP_TOKEN_COOKIE_ENCRYPTION ".cookie"
#define SP_TOKEN_SESSION_ENCRYPTION ".session"
#define SP_TOKEN_DISABLE_FUNC ".disable_function"
#define SP_TOKEN_GLOBAL ".global"
#define SP_TOKEN_GLOBAL_STRICT ".global_strict"
#define SP_TOKEN_HARDEN_RANDOM ".harden_random"
#define SP_TOKEN_READONLY_EXEC ".readonly_exec"
#define SP_TOKEN_UNSERIALIZE_HMAC ".unserialize_hmac"
#define SP_TOKEN_UPLOAD_VALIDATION ".upload_validation"
#define SP_TOKEN_DISABLE_XXE ".disable_xxe"
#define SP_TOKEN_EVAL_BLACKLIST ".eval_blacklist"
#define SP_TOKEN_EVAL_WHITELIST ".eval_whitelist"
#define SP_TOKEN_SLOPPY_COMPARISON ".sloppy_comparison"
#define SP_TOKEN_ALLOW_WRAPPERS ".wrappers_whitelist"

// common tokens
#define SP_TOKEN_ENABLE ".enable("
#define SP_TOKEN_DISABLE ".disable("
#define SP_TOKEN_SIMULATION ".simulation("
#define SP_TOKEN_TRUE "1"
#define SP_TOKEN_FALSE "0"
#define SP_TOKEN_DUMP ".dump("
#define SP_TOKEN_ALIAS ".alias("
#define SP_TOKEN_ALLOW ".allow("
#define SP_TOKEN_DROP ".drop("

#define SP_TOKEN_END_PARAM ')'

// disable_function
#define SP_TOKEN_CIDR ".cidr("
#define SP_TOKEN_FILENAME ".filename("
#define SP_TOKEN_FILENAME_REGEXP ".filename_r("
#define SP_TOKEN_FUNCTION ".function("
#define SP_TOKEN_FUNCTION_REGEXP ".function_r("
#define SP_TOKEN_HASH ".hash("
#define SP_TOKEN_LOCAL_VAR ".var("
#define SP_TOKEN_PARAM ".param("
#define SP_TOKEN_PARAM_REGEXP ".param_r("
#define SP_TOKEN_PARAM_TYPE ".param_type("
#define SP_TOKEN_RET ".ret("
#define SP_TOKEN_RET_REGEXP ".ret_r("
#define SP_TOKEN_RET_TYPE ".ret_type("
#define SP_TOKEN_VALUE ".value("
#define SP_TOKEN_VALUE_REGEXP ".value_r("
#define SP_TOKEN_KEY ".key("
#define SP_TOKEN_KEY_REGEXP ".key_r("
#define SP_TOKEN_VALUE_ARG_POS ".pos("
#define SP_TOKEN_LINE_NUMBER ".line("

// cookies encryption
#define SP_TOKEN_NAME ".name("
#define SP_TOKEN_NAME_REGEXP ".name_r("

// cookies samesite
#define SP_TOKEN_SAMESITE ".samesite("
#define SP_TOKEN_ENCRYPT ".encrypt("
#define SP_TOKEN_SAMESITE_LAX "Lax"
#define SP_TOKEN_SAMESITE_STRICT "Strict"

// Global configuration options
#define SP_TOKEN_ENCRYPTION_KEY ".secret_key("
#define SP_TOKEN_ENV_VAR ".cookie_env_var("
#define SP_TOKEN_LOG_MEDIA ".log_media("

// upload_validator
#define SP_TOKEN_UPLOAD_SCRIPT ".script("

#define SP_TOKEN_LIST ".list("

int sp_parse_config(const char *);
int parse_array(sp_disabled_function *);

int parse_str(char *restrict, char *restrict, void *);
int parse_regexp(char *restrict, char *restrict, void *);
int parse_empty(char *restrict, char *restrict, void *);
int parse_cidr(char *restrict, char *restrict, void *);
int parse_php_type(char *restrict, char *restrict, void *);
int parse_list(char *restrict, char *restrict, void *);

// cleanup
void sp_disabled_function_list_free(sp_list_node *);

#endif /* SP_CONFIG_H */
