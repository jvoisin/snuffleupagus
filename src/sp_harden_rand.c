#include "php_snuffleupagus.h"

extern ZEND_API zend_class_entry *zend_ce_error;

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

/* This function is needed because `rand` and `mt_rand` parameters
 * are optional, while the ones from `random_int` aren't. */
static void random_int_wrapper(INTERNAL_FUNCTION_PARAMETERS) {
  zend_long min, max, result;

  switch (EX_NUM_ARGS()) {
  case 0:
    min = 0;
    max = PHP_MT_RAND_MAX;
    break;
  case 1:
    // LCOV_EXCL_BR_START
    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_QUIET, 1, 1);
    Z_PARAM_LONG(min);
    ZEND_PARSE_PARAMETERS_END();
    // LCOV_EXCL_BR_END
    max = PHP_MT_RAND_MAX;
    break;
  case 2:
  default:
    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_QUIET, 0, 2);
    Z_PARAM_LONG(min);
    Z_PARAM_LONG(max);
    ZEND_PARSE_PARAMETERS_END();
  }

  if (min > max) {
    if (php_random_int_throw(max, min, &result) == FAILURE) {
      return;
    }
  } else {
    if (php_random_int_throw(min, max, &result) == FAILURE) {
      return;
    }
  }

  RETURN_LONG(result);
}

PHP_FUNCTION(sp_rand) {
  void (*orig_handler)(INTERNAL_FUNCTION_PARAMETERS);

  if ((orig_handler = zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(sp_internal_functions_hook), "rand",
                                             strlen("rand")))) {
    /* call the original `rand` function,
     * since we might no be the only ones to hook it*/
    orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  } else
    sp_log_err("harden_rand", "Unable to find the pointer to the original function 'rand' in the hashtable.\n");
  random_int_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(sp_mt_rand) {
  void (*orig_handler)(INTERNAL_FUNCTION_PARAMETERS);

  if ((orig_handler = zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(sp_internal_functions_hook),
                                             "mt_rand", strlen("mt_rand")))) {
    /* call the original `mt_rand` function,
     * since we might no be the only ones to hook it*/
    orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  } else
    sp_log_err("harden_rand", "Unable to find the pointer to the original function 'mt_rand' in the hashtable.\n");

  random_int_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

int hook_rand() {
  TSRMLS_FETCH();

  HOOK_FUNCTION("rand", sp_internal_functions_hook, PHP_FN(sp_rand), false);
  HOOK_FUNCTION("mt_rand", sp_internal_functions_hook, PHP_FN(sp_mt_rand), false);

  return SUCCESS;
}
