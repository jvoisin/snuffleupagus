#include "php_snuffleupagus.h"

sp_pcre* sp_pcre_compile(const char* const pattern) {
  assert(NULL != pattern);

  unsigned char pcre_error[128] = {0};
  int errornumber;
  PCRE2_SIZE erroroffset;
  sp_pcre* ret = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
                      PCRE2_CASELESS, &errornumber, &erroroffset, NULL);

  if (NULL == ret) {
    pcre2_get_error_message(errornumber, pcre_error, sizeof(pcre_error));
    sp_log_err("config", "Failed to compile '%s': %s on line %zu.", pattern,
               pcre_error, sp_line_no);
  }
  return ret;
}

bool ZEND_HOT sp_is_regexp_matching_len(const sp_pcre* regexp, const char* str,
                                        size_t len) {
  assert(NULL != regexp);
  assert(NULL != str);

  pcre2_match_data* match_data =
      pcre2_match_data_create_from_pattern(regexp, NULL);
  int ret = pcre2_match(regexp, (PCRE2_SPTR)str, len, 0, 0, match_data, NULL);

  if (ret < 0) {
    if (ret != PCRE2_ERROR_NOMATCH) {
      // LCOV_EXCL_START
      sp_log_err("regexp", "Something went wrong with a regexp (%d).", ret);
      // LCOV_EXCL_STOP
    }
    return false;
  }
  return true;
}
