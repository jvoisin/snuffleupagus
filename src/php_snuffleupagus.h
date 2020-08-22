#ifndef PHP_SNUFFLEUPAGUS_H
#define PHP_SNUFFLEUPAGUS_H

#define PHP_SNUFFLEUPAGUS_VERSION "0.5.1"
#define PHP_SNUFFLEUPAGUS_EXTNAME "snuffleupagus"
#define PHP_SNUFFLEUPAGUS_AUTHOR "NBS System"
#define PHP_SNUFFLEUPAGUS_URL "https://github.com/jvoisin/snuffleupagus"
#define PHP_SNUFFLEUPAGUS_COPYRIGHT "LGPLv2"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
#include "ext/session/php_session.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"
#include "ext/standard/url.h"
#include "ext/standard/php_var.h"
#include "ext/pcre/php_pcre.h"
#include "ext/session/php_session.h"
#include "php.h"
#include "php_ini.h"
#include "rfc1867.h"
#include "zend_execute.h"
#include "zend_extensions.h"
#include "zend_hash.h"
#include "zend_string.h"
#include "zend_types.h"
#include "zend_vm.h"

/* Compatibility */
#if ( !HAVE_PCRE && !HAVE_BUNDLED_PCRE )
#error Snuffleupagus requires PHP7+ with PCRE support
#endif
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
#endif

#define SP_CONFIG_VALID 1
#define SP_CONFIG_INVALID 0
#define SP_CONFIG_NONE -1

#include "sp_pcre_compat.h"
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
#include "sp_crypt.h"
#include "sp_session.h"
#include "sp_sloppy.h"
#include "sp_wrapper.h"

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
int is_config_valid;  // 1 = valid, 0 = invalid, -1 = none
bool allow_broken_configuration;
HashTable *disabled_functions_hook;
HashTable *sp_internal_functions_hook;
HashTable *sp_eval_blacklist_functions_hook;
ZEND_END_MODULE_GLOBALS(snuffleupagus)

ZEND_EXTERN_MODULE_GLOBALS(snuffleupagus)
#define SNUFFLEUPAGUS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(snuffleupagus, v)

#if defined(ZTS) && defined(COMPILE_DL_SNUFFLEUPAGUS)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

PHP_FUNCTION(check_disabled_function);
PHP_FUNCTION(eval_blacklist_callback);

#endif /* PHP_SNUFFLEUPAGUS_H */
