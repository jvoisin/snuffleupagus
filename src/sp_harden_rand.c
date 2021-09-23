#include "php_snuffleupagus.h"

extern ZEND_API zend_class_entry *zend_ce_error;

/* This function is needed because `rand` and `mt_rand` parameters
 * are optional, while the ones from `random_int` aren't. */
static void random_int_wrapper(INTERNAL_FUNCTION_PARAMETERS) {
  zend_long min = 0;
  zend_long max = PHP_MT_RAND_MAX;
  zend_long result;

  switch (EX_NUM_ARGS()) {
    case 0:
      min = 0;
      max = PHP_MT_RAND_MAX;
      break;
    case 1:
      // LCOV_EXCL_BR_START
      ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_QUIET, 1, 1);
      Z_PARAM_LONG(min);
      /* ZEND_PARSE_PARAMETERS_END call ZEND_PARSE_PARAMETERS_END_EX with
       * `return` as a callback. As we don't need to strictly parse all
       * parameters,
       * we call ZEMD_PARSE_PARAMETERS_END_EX with (void)0 as a callback.
       * If things go wrong, `php_random_int_throw` will scream anyway.
       * There might be a better way to do it, please tell us if you know. */
      ZEND_PARSE_PARAMETERS_END_EX((void)0);
      // LCOV_EXCL_BR_END
      max = PHP_MT_RAND_MAX;
      break;
    case 2:
    default:
      ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_QUIET, 2, 2);
      Z_PARAM_LONG(min);
      Z_PARAM_LONG(max);
      ZEND_PARSE_PARAMETERS_END_EX((void)0);
  }

  if (min > max) {
    if (php_random_int_throw(max, min, &result) == FAILURE) {
      return;  // LCOV_EXCL_LINE
    }
  } else {
    if (php_random_int_throw(min, max, &result) == FAILURE) {
      return;  // LCOV_EXCL_LINE
    }
  }

  RETURN_LONG(result);
}

PHP_FUNCTION(sp_rand) {
  zif_handler orig_handler;

  /* call the original `rand` function,
   * since we might no be the only ones to hook it*/
  orig_handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("rand"));
  orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

  random_int_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(sp_mt_rand) {
  zif_handler orig_handler;

  /* call the original `mt_rand` function,
   * since we might no be the only ones to hook it*/
  orig_handler =
      zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("mt_rand"));
  orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

  random_int_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

int hook_rand() {
  TSRMLS_FETCH();

  HOOK_FUNCTION("rand", sp_internal_functions_hook, PHP_FN(sp_rand));
  HOOK_FUNCTION("mt_rand", sp_internal_functions_hook, PHP_FN(sp_mt_rand));

  return SUCCESS;
}
