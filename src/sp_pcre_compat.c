#include "php_snuffleupagus.h"

inline void sp_pcre_free(sp_pcre* regexp) {
#ifdef SP_HAS_PCRE2
  pcre2_code_free(regexp);
#else
  (void)regexp;
#endif
}

sp_pcre* sp_pcre_compile(const char* const pattern) {
  assert(NULL != pattern);

  sp_pcre* ret = NULL;
#ifdef SP_HAS_PCRE2
  unsigned char pcre_error[128] = {0};
  int errornumber;
  PCRE2_SIZE erroroffset;
  ret = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
                      PCRE2_CASELESS, &errornumber, &erroroffset, NULL);
  pcre2_get_error_message(errornumber, pcre_error, sizeof(pcre_error));
#else
  const char* pcre_error = NULL;
  int erroroffset;
  ret = pcre_compile(pattern, PCRE_CASELESS, &pcre_error, &erroroffset, NULL);
#endif

  if (NULL == ret) {
    sp_log_err("config", "Failed to compile '%s': %s on line %zu.", pattern,
               pcre_error, sp_line_no);
  }
  return ret;
}

bool ZEND_HOT sp_is_regexp_matching_len(const sp_pcre* regexp, const char* str,
                                        size_t len) {
  int ret = 0;

  assert(NULL != regexp);
  assert(NULL != str);

#ifdef SP_HAS_PCRE2
  pcre2_match_data* match_data =
      pcre2_match_data_create_from_pattern(regexp, NULL);
  if (NULL == match_data) {
    sp_log_err("regexp", "Unable to get memory for a regxp.");
  }
  ret = pcre2_match(regexp, (PCRE2_SPTR)str, len, 0, 0, match_data, NULL);
  pcre2_match_data_free(match_data);
#else
  int vec[30];
  ret = pcre_exec(regexp, NULL, str, len, 0, 0, vec, sizeof(vec) / sizeof(int));
#endif

  if (ret < 0) {
#ifdef SP_HAS_PCRE2
    if (ret != PCRE2_ERROR_NOMATCH) {
#else
    if (ret != PCRE_ERROR_NOMATCH) {
#endif
      // LCOV_EXCL_START
      sp_log_err("regexp", "Something went wrong with a regexp (%d).", ret);
      // LCOV_EXCL_STOP
    }
    return false;
  }
  return true;
}
