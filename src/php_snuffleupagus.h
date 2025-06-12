#ifndef PHP_SNUFFLEUPAGUS_H
#define PHP_SNUFFLEUPAGUS_H

#define PHP_SNUFFLEUPAGUS_VERSION "0.11.0"
#define PHP_SNUFFLEUPAGUS_EXTNAME "snuffleupagus"
#define PHP_SNUFFLEUPAGUS_AUTHOR "NBS System & Julien (jvoisin) Voisin & SektionEins GmbH"
#define PHP_SNUFFLEUPAGUS_URL "https://github.com/jvoisin/snuffleupagus"
#define PHP_SNUFFLEUPAGUS_COPYRIGHT "LGPLv3"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef PHP_WIN32
#include "win32/glob.h"
#else
#include <glob.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syslog.h>

#include "SAPI.h"
#include "ext/pcre/php_pcre.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"
#include "ext/standard/url.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_var.h"
#include "ext/session/php_session.h"
#include "ext/hash/php_hash.h"
#include "php.h"
#include "php_ini.h"
#include "rfc1867.h"
#include "zend_execute.h"
#include "zend_extensions.h"
#include "zend_hash.h"
#include "zend_string.h"
#include "zend_smart_str.h"
#include "zend_types.h"
#include "zend_vm.h"

/* Compatibility */
#if PHP_VERSION_ID < 70000
#error Snuffleupagus only works with PHP7+. You shouldn't use PHP5 anyway, \
  since it's not supported anymore: https://secure.php.net/supported-versions.php
#endif

#if PHP_VERSION_ID < 70200
typedef void (*zif_handler)(INTERNAL_FUNCTION_PARAMETERS);
#endif

#if PHP_VERSION_ID >= 80000
#define TSRMLS_FETCH()
#define TSRMLS_C
#else
#if (!HAVE_PCRE && !HAVE_BUNDLED_PCRE)
#error Snuffleupagus requires PHP7+ with PCRE support
#endif
#endif

#if PHP_VERSION_ID < 80200
#include "ext/standard/php_rand.h"
#include "ext/standard/php_random.h"
#else
#include "ext/random/php_random.h"
#endif

#define SP_CONFIG_VALID 1
#define SP_CONFIG_INVALID 0
#define SP_CONFIG_NONE -1

#include "sp_php_compat.h"
#include "sp_pcre_compat.h"
#include "sp_list.h"
#include "sp_tree.h"
#include "sp_var_parser.h"
#include "sp_config_scanner.h"
#include "sp_config.h"
#include "sp_config_utils.h"
#include "sp_config_keywords.h"
#include "sp_cookie_encryption.h"
#include "sp_disable_xxe.h"
#include "sp_disabled_functions.h"
#include "sp_execute.h"
#include "sp_harden_rand.h"
#include "sp_network_utils.h"
#include "sp_unserialize.h"
#include "sp_upload_validation.h"
#include "sp_utils.h"
#include "sp_crypt.h"
#include "sp_session.h"
#include "sp_sloppy.h"
#include "sp_wrapper.h"
#include "sp_ini.h"
#include "sp_ifilter.h"

extern zend_module_entry snuffleupagus_module_entry;
#define phpext_snuffleupagus_ptr &snuffleupagus_module_entry

#ifdef PHP_WIN32
#define PHP_SNUFFLEUPAGUS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_SNUFFLEUPAGUS_API __attribute__((visibility("default")))
#else
#define PHP_SNUFFLEUPAGUS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(snuffleupagus)
    sp_pcre *global_regexp_const; \

    sp_pcre *global_regexp_var;
// sp_config config;
// --- snuffleupagus config
sp_config_random config_random;
sp_config_sloppy config_sloppy;
sp_config_unserialize config_unserialize;
sp_config_unserialize_noclass config_unserialize_noclass;
sp_config_readonly_exec config_readonly_exec;
sp_config_upload_validation config_upload_validation;
sp_config_cookie config_cookie;
sp_config_auto_cookie_secure config_auto_cookie_secure;
sp_config_global_strict config_global_strict;
sp_config_xxe_protection config_xxe_protection;
sp_config_eval config_eval;
sp_config_wrapper config_wrapper;
sp_config_session config_session;
sp_config_ini config_ini;
char config_log_media;
int config_log_max_len;
u_long config_max_execution_depth;
bool config_server_encode;
bool config_server_strip;
zend_string *config_encryption_key;
zend_string *config_cookies_env_var;

HashTable *config_disabled_functions;
HashTable *config_disabled_functions_hooked;
HashTable *config_disabled_functions_ret;
HashTable *config_disabled_functions_ret_hooked;
sp_config_disabled_functions config_disabled_functions_reg;
sp_config_disabled_functions config_disabled_functions_reg_ret;

bool hook_execute;

// --- ini options
bool allow_broken_configuration;

// --- runtime/state variables
int is_config_valid;  // 1 = valid, 0 = invalid, -1 = none
size_t in_eval;
u_long execution_depth;
HashTable *disabled_functions_hook;
HashTable *sp_internal_functions_hook;
HashTable *sp_eval_blacklist_functions_hook;

#if PHP_VERSION_ID >= 80000
const zend_string* eval_source_string;
#else
const zval* eval_source_string;
#endif

ZEND_END_MODULE_GLOBALS(snuffleupagus)

ZEND_EXTERN_MODULE_GLOBALS(snuffleupagus)
#define SNUFFLEUPAGUS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(snuffleupagus, v)
#define SPG(v) SNUFFLEUPAGUS_G(v)
#define SPCFG(v) SPG(config_##v)

#if defined(ZTS) && defined(COMPILE_DL_SNUFFLEUPAGUS)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

PHP_FUNCTION(check_disabled_function);
PHP_FUNCTION(eval_blacklist_callback);

#endif /* PHP_SNUFFLEUPAGUS_H */
