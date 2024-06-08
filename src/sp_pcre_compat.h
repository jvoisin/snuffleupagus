#ifndef SP_PCRE_COMPAT_H
#define SP_PCRE_COMPAT_H

#include <stdlib.h>
#include <stdbool.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#if PHP_VERSION_ID >= 70300
#define SP_HAS_PCRE2
#endif
#include "ext/pcre/php_pcre.h"  // PCRE1

#ifdef SP_HAS_PCRE2
#define sp_pcre pcre2_code
#else
#define sp_pcre pcre
#endif

sp_pcre* sp_pcre_compile(const char* str);
static inline void sp_pcre_free(sp_pcre* regexp) {
#ifdef SP_HAS_PCRE2
  pcre2_code_free(regexp);
#endif
}
bool sp_is_regexp_matching_len(const sp_pcre* regexp, const char* str, size_t len);


typedef struct {
  sp_pcre *re;
  zend_string *pattern;
} sp_regexp;

#define sp_is_regexp_matching_zstr(regexp, zstr) sp_is_regexp_matching_len(regexp->re, ZSTR_VAL(zstr), ZSTR_LEN(zstr))
#define sp_is_regexp_matching(regexp, str) sp_is_regexp_matching_len(regexp->re, str, strlen(str))
static inline sp_regexp* sp_regexp_compile(zend_string *zstr) {
  sp_pcre *re = sp_pcre_compile(ZSTR_VAL(zstr));
  if (!re) { return NULL; }
  sp_regexp *ret = pecalloc(1, sizeof(sp_regexp), 1);
  ret->re = re;
  ret->pattern = zstr;
  return ret;
}
static inline void sp_regexp_free(sp_regexp *regexp) {
  if (regexp) {
    if (regexp->re) { sp_pcre_free(regexp->re); }
    if (regexp->pattern) { zend_string_release(regexp->pattern); }
    pefree(regexp, 1);
  }
}


#endif  // SP_PCRE_COMPAT_H
