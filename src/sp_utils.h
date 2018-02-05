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

#define HOOK_FUNCTION(original_name, hook_table, new_function, execution) \
  hook_function(original_name, SNUFFLEUPAGUS_G(hook_table), new_function, \
                execution)

#define HOOK_FUNCTION_BY_REGEXP(regexp, hook_table, new_function, execution) \
  hook_regexp(regexp, SNUFFLEUPAGUS_G(hook_table), new_function, execution)

#define SP_LOG_SIMULATION "simulation"
#define SP_LOG_DROP "drop"
#define SP_LOG_DEBUG "debug"
#define SP_LOG_ERROR "error"

#define sp_log_err(feature, ...) sp_log_msg(feature, SP_LOG_ERROR, __VA_ARGS__)
#ifdef SP_DEBUG
#define sp_log_debug(...) sp_log_msg("DEBUG", SP_LOG_DEBUG, __VA_ARGS__)
#else
#define sp_log_debug(...)
#endif

#define GET_SUFFIX(x) (x == 1) ? "st" : ((x == 2) ? "nd" : "th")

void sp_log_msg(char const *feature, char const *level, const char *fmt, ...);
int compute_hash(const char *const filename, char *file_hash);
char *sp_convert_to_string(zval *);
bool sp_match_value(const char *, const char *, const sp_pcre *);
bool sp_match_array_key(const zval *, const char *, const sp_pcre *);
bool sp_match_array_value(const zval *, const char *, const sp_pcre *);
void sp_log_disable(const char *restrict, const char *restrict,
                    const char *restrict, const sp_disabled_function *);
void sp_log_disable_ret(const char *restrict, const char *restrict,
                        const sp_disabled_function *);
int hook_function(const char *, HashTable *,
                  void (*)(INTERNAL_FUNCTION_PARAMETERS), bool);
int hook_regexp(const sp_pcre *, HashTable *,
                void (*)(INTERNAL_FUNCTION_PARAMETERS), bool);
bool check_is_in_eval_whitelist(const char * const function_name);

#endif /* SP_UTILS_H */
