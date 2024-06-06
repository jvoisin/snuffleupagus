#ifndef SP_UTILS_H
#define SP_UTILS_H

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/standard/md5.h"

#include "sp_config.h"
#include "sp_list.h"

#if defined(__GNUC__)
#if __GNUC__ >= 3
#define sp_pure __attribute__((pure))
#define sp_const __attribute__((const))
#else
#define sp_pure
#define sp_const
#endif
#endif

#define VAR_AND_LEN(var) var, strlen(var)

#define SHA256_SIZE 32

#define HOOK_FUNCTION(original_name, hook_table, new_function) \
  hook_function(original_name, SPG(hook_table), new_function)

#define HOOK_FUNCTION_BY_REGEXP(regexp, hook_table, new_function) \
  hook_regexp(regexp, SPG(hook_table), new_function)

#define SP_TYPE_LOG (0)
#define SP_TYPE_DROP (1)
#define SP_TYPE_SIMULATION (2)

#define SP_LOG_DEBUG E_NOTICE
#define SP_LOG_INFO E_NOTICE
#define SP_LOG_ERROR E_ERROR
#define SP_LOG_WARN E_WARNING

#define sp_log_msg(feature, level, ...) \
  sp_log_msgf(feature, level, SP_TYPE_LOG, __VA_ARGS__)
#define sp_log_drop(feature, ...) \
  sp_log_msgf(feature, SP_LOG_ERROR, SP_TYPE_DROP, __VA_ARGS__)
#define sp_log_simulation(feature, ...) \
  sp_log_msgf(feature, SP_LOG_WARN, SP_TYPE_SIMULATION, __VA_ARGS__)
#define sp_log_auto(feature, is_simulation, ...)                     \
  sp_log_msgf(feature, (is_simulation ? SP_LOG_WARN : SP_LOG_ERROR), \
              (is_simulation ? SP_TYPE_SIMULATION : SP_TYPE_DROP),   \
              __VA_ARGS__)

#define sp_log_err(feature, ...) \
  sp_log_msgf(feature, SP_LOG_ERROR, SP_TYPE_LOG, __VA_ARGS__)
#define sp_log_warn(feature, ...) \
  sp_log_msgf(feature, SP_LOG_WARN, SP_TYPE_LOG, __VA_ARGS__)

#ifdef SP_DEBUG

#ifdef SP_DEBUG_STDERR
extern int sp_debug_stderr;
#define sp_log_debug(fmt, ...) \
  if (sp_debug_stderr > 0) dprintf(sp_debug_stderr, "[snuffleupagus][DEBUG] %s(): " fmt "\n", __FUNCTION__, ##__VA_ARGS__);
#else
#define sp_log_debug(fmt, ...) \
  sp_log_msgf("DEBUG", SP_LOG_DEBUG, SP_TYPE_LOG, "%s(): " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif

#else
#define sp_log_debug(...)
#endif

#define GET_SUFFIX(x) (x == 1) ? "st" : ((x == 2) ? "nd" : "th")

const char *get_ipaddr(void);
#if defined __has_attribute
#  if __has_attribute (__format__)
     __attribute__((__format__(printf, 4, 5)))
#  endif
#endif
void sp_log_msgf(char const* const restrict feature, int level, int type, char const* const restrict fmt, ...);
int compute_hash(char const* const restrict filename, char *restrict file_hash);
const zend_string *sp_zval_to_zend_string(const zval *);
bool sp_match_value(const zend_string* value, const zend_string* to_match, const sp_regexp* rx);
bool sp_match_array_key(const zval *, const zend_string *, const sp_regexp *);
bool sp_match_array_value(const zval *, const zend_string *, const sp_regexp *);
void sp_log_disable(const char *restrict, const char *restrict, const zend_string *restrict, const sp_disabled_function *);
void sp_log_disable_ret(const char *restrict, const zend_string *restrict, const sp_disabled_function *);
bool hook_function(const char *, HashTable *, zif_handler);
void unhook_functions(HashTable *ht);
int hook_regexp(const sp_pcre *, HashTable *, zif_handler);
bool check_is_in_eval_whitelist(const char* function_name);
int sp_log_request(zend_string const* const restrict folder, zend_string const* const restrict text_repr);
#define sp_zend_string_equals(s1, s2) zend_string_equals((const zend_string*)s1, (const zend_string*)s2)
static inline bool sp_zend_string_equals_str(const zend_string* s1, const char *str, size_t len) {
  return (ZSTR_LEN(s1) == len && !memcmp(ZSTR_VAL(s1), str, len));
}
#endif /* SP_UTILS_H */
