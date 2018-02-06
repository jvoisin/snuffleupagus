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

typedef struct {
  int ip_version;
  union {
    struct in_addr ipv4;
    struct in6_addr ipv6;
  } ip;
  uint8_t mask;
} sp_cidr;

typedef struct {
  char *encryption_key;
  char *cookies_env_var;
} sp_config_global;

typedef struct {
  bool enable;
  bool simulation;
} sp_config_readonly_exec;

typedef struct { bool enable; } sp_config_global_strict;

typedef struct { bool enable; } sp_config_random;

typedef struct { bool enable; } sp_config_auto_cookie_secure;

typedef struct { bool enable; } sp_config_disable_xxe;

typedef struct {
  enum samesite_type { strict = 1, lax = 2 } samesite;
  bool encrypt;
  char *name;
  pcre *name_r;
  bool simulation;
} sp_cookie;

typedef struct {
  bool enable;
  bool simulation;
} sp_config_unserialize;

typedef struct {
  char *textual_representation;

  char *filename;
  pcre *r_filename;

  char *function;
  pcre *r_function;
  sp_list_node *functions_list;

  char *hash;
  int simulation;

  sp_tree *param;
  pcre *r_param;
  sp_php_type param_type;
  int pos;
  unsigned int line;

  char *ret;
  pcre *r_ret;
  sp_php_type ret_type;

  pcre *value_r;
  char *value;

  pcre *r_key;
  char *key;

  char *dump;
  char *alias;
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
} sp_config_eval;

typedef struct {
  sp_list_node *disabled_functions;  // list of sp_disabled_function
} sp_config_disabled_functions;

typedef struct {
  sp_list_node *cookies;  // list of sp_cookie for regexp/names
} sp_config_cookie;

typedef struct {
  sp_list_node
      *construct_include;  // list of rules for `(include|require)_(once)?`
  sp_list_node *construct_eval;
  sp_list_node *construct_echo;
} sp_config_disabled_constructs;

typedef struct {
  char *script;
  bool simulation;
  bool enable;
} sp_config_upload_validation;

typedef struct {
  sp_config_random *config_random;
  sp_config_unserialize *config_unserialize;
  sp_config_disabled_functions *config_disabled_functions;
  sp_config_disabled_functions *config_disabled_functions_ret;
  sp_config_readonly_exec *config_readonly_exec;
  sp_config_upload_validation *config_upload_validation;
  sp_config_cookie *config_cookie;
  sp_config_global *config_snuffleupagus;
  sp_config_auto_cookie_secure *config_auto_cookie_secure;
  sp_config_global_strict *config_global_strict;
  sp_config_disable_xxe *config_disable_xxe;
  sp_config_disabled_constructs *config_disabled_constructs;
  sp_config_eval *config_eval;
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

// upload_validator
#define SP_TOKEN_UPLOAD_SCRIPT ".script("

// eval blacklist
#define SP_TOKEN_EVAL_LIST ".list("

int sp_parse_config(const char *);
int parse_array(sp_disabled_function *);

int parse_str(char *restrict, char *restrict, void *);
int parse_regexp(char *restrict, char *restrict, void *);
int parse_empty(char *restrict, char *restrict, void *);
int parse_cidr(char *restrict, char *restrict, void *);
int parse_php_type(char *restrict, char *restrict, void *);

// cleanup
void sp_disabled_function_list_free(sp_list_node *);

#endif /* SP_CONFIG_H */
