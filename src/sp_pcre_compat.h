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
#define sp_is_regexp_matching_zend(regexp, zstr) \
  sp_is_regexp_matching_len(regexp, ZSTR_VAL(zstr), ZSTR_LEN(zstr))
#define sp_is_regexp_matching(regexp, str) \
  sp_is_regexp_matching_len(regexp, str, strlen(str))
bool sp_is_regexp_matching_len(const sp_pcre* regexp, const char* str,
                               size_t len);

#endif  // SP_PCRE_COMPAT_H
