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
  hook_function(original_name, SNUFFLEUPAGUS_G(hook_table), new_function)

#define HOOK_FUNCTION_BY_REGEXP(regexp, hook_table, new_function) \
  hook_regexp(regexp, SNUFFLEUPAGUS_G(hook_table), new_function)

#define SP_LOG_SIMULATION 0x100000
#define SP_LOG_DROP 0x200000
#define SP_LOG_DEBUG E_NOTICE
#define SP_LOG_ERROR E_ERROR
#define SP_LOG_WARN E_WARNING

#define sp_log_err(feature, ...) sp_log_msg(feature, SP_LOG_ERROR, __VA_ARGS__)
#define sp_log_warn(feature, ...) sp_log_msg(feature, SP_LOG_WARN, __VA_ARGS__)
#ifdef SP_DEBUG
#define sp_log_debug(...) sp_log_msg("DEBUG", SP_LOG_DEBUG, __VA_ARGS__)
#else
#define sp_log_debug(...)
#endif

#define GET_SUFFIX(x) (x == 1) ? "st" : ((x == 2) ? "nd" : "th")

const char *get_ipaddr();
void sp_log_msg(char const *restrict feature, int type,
                const char *restrict fmt, ...);
int compute_hash(const char *const restrict filename, char *restrict file_hash);
const zend_string *sp_zval_to_zend_string(const zval *);
bool sp_match_value(const zend_string *, const zend_string *, const sp_pcre *);
bool sp_match_array_key(const zval *, const zend_string *, const sp_pcre *);
bool sp_match_array_value(const zval *, const zend_string *, const sp_pcre *);
void sp_log_disable(const char *restrict, const char *restrict,
                    const zend_string *restrict, const sp_disabled_function *);
void sp_log_disable_ret(const char *restrict, const zend_string *restrict,
                        const sp_disabled_function *);
int hook_function(const char *, HashTable *, zif_handler);
int hook_regexp(const sp_pcre *, HashTable *, zif_handler);
bool check_is_in_eval_whitelist(const zend_string *const function_name);
int sp_log_request(const zend_string *restrict folder,
                   const zend_string *restrict text_repr, char *from);
bool sp_zend_string_equals(const zend_string *s1, const zend_string *s2);

#endif /* SP_UTILS_H */
