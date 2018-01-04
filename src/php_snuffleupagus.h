#ifndef PHP_SNUFFLEUPAGUS_H
#define PHP_SNUFFLEUPAGUS_H

#define PHP_SNUFFLEUPAGUS_COMMIT ""
#define PHP_SNUFFLEUPAGUS_VERSION "0.1 " PHP_SNUFFLEUPAGUS_COMMIT
#define PHP_SNUFFLEUPAGUS_EXTNAME "snuffleupagus"
#define PHP_SNUFFLEUPAGUS_AUTHOR "NBS System"
#define PHP_SNUFFLEUPAGUS_URL "https://github.com/nbs-system/snuffleupagus"
#define PHP_SNUFFLEUPAGUS_COPYRIGHT "LGPLv2"

#include <stdbool.h>
#include <stdio.h>

#include <pcre.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "SAPI.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "ext/pcre/php_pcre.h"
#include "php.h"
#include "php_ini.h"
#include "zend_hash.h"
#include "zend_string.h"
#include "zend_extensions.h"

#include "sp_list.h"
#include "sp_tree.h"
#include "sp_var_parser.h"
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
size_t in_eval;
sp_config config;
bool is_config_valid;
HashTable *disabled_functions_hook;
HashTable *sp_internal_functions_hook;
HashTable *sp_eval_blacklist_functions_hook;
ZEND_END_MODULE_GLOBALS(snuffleupagus)

#define SNUFFLEUPAGUS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(snuffleupagus, v)

#if defined(ZTS) && defined(COMPILE_DL_SNUFFLEUPAGUS)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#if HAVE_BUNDLED_PCRE
 #include "ext/pcre/pcrelib/pcre.h"
 #undef pcre_exec
 #undef pcre_compile
 #define sp_pcre_exec pcre_exec
 #define sp_pcre_compile pcre_compile
#else
 #include "pcre.h"
 #define sp_pcre_exec pcre_exec
 #define sp_pcre_compile pcre_compile
#endif

PHP_FUNCTION(check_disabled_function);
PHP_FUNCTION(eval_blacklist_callback);

static inline void sp_terminate() { zend_bailout(); }

#endif /* PHP_SNUFFLEUPAGUS_H */
