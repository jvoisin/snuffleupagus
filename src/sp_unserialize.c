#include "php_snuffleupagus.h"

PHP_FUNCTION(sp_serialize) {
  zif_handler orig_handler;

  /* Call the original `serialize` function. */
  orig_handler =
      zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(sp_internal_functions_hook),
                             "serialize", sizeof("serialize") - 1);
  orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

  /* Compute the HMAC of the textual representation of the serialized data*/
  zval func_name;
  zval hmac;
  zval params[3];

  ZVAL_STRING(&func_name, "hash_hmac");
  ZVAL_STRING(&params[0], "sha256");
  params[1] = *return_value;
  ZVAL_STRING(
      &params[2],
      ZSTR_VAL(SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key));
  call_user_function(CG(function_table), NULL, &func_name, &hmac, 3, params);

  size_t len = Z_STRLEN_P(return_value) + Z_STRLEN(hmac);
  if (len < Z_STRLEN_P(return_value)) {
    // LCOV_EXCL_START
    sp_log_err("overflow_error",
               "Overflow tentative detected in sp_serialize.");
    zend_bailout();
    // LCOV_EXCL_STOP
  }
  zend_string *res = zend_string_alloc(len, 0);

  memcpy(ZSTR_VAL(res), Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
  memcpy(ZSTR_VAL(res) + Z_STRLEN_P(return_value), Z_STRVAL(hmac),
         Z_STRLEN(hmac));

  /* Append the computed HMAC to the serialized data. */
  return_value->value.str = res;
  return;
}

PHP_FUNCTION(sp_unserialize) {
  zif_handler orig_handler;

  char *buf = NULL;
  char *serialized_str = NULL;
  char *hmac = NULL;
  zval expected_hmac;
  size_t buf_len = 0;
  zval *opts = NULL;

  const sp_config_unserialize *config_unserialize =
      SNUFFLEUPAGUS_G(config).config_unserialize;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|a", &buf, &buf_len, &opts) ==
      FAILURE) {
    RETURN_FALSE;
  }

  /* 64 is the length of HMAC-256 */
  if (buf_len < 64) {
    sp_log_msg("unserialize", SP_LOG_DROP,
               "The serialized object is too small.");
  }

  hmac = buf + buf_len - 64;
  serialized_str = ecalloc(buf_len - 64 + 1, 1);
  memcpy(serialized_str, buf, buf_len - 64);

  zval func_name;
  ZVAL_STRING(&func_name, "hash_hmac");

  zval params[3];
  ZVAL_STRING(&params[0], "sha256");
  ZVAL_STRING(&params[1], serialized_str);
  ZVAL_STRING(
      &params[2],
      ZSTR_VAL(SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key));
  call_user_function(CG(function_table), NULL, &func_name, &expected_hmac, 3,
                     params);

  unsigned int status = 0;
  for (uint8_t i = 0; i < 64; i++) {
    status |= (hmac[i] ^ (Z_STRVAL(expected_hmac))[i]);
  }

  if (0 == status) {
    if ((orig_handler = zend_hash_str_find_ptr(
             SNUFFLEUPAGUS_G(sp_internal_functions_hook), "unserialize",
             sizeof("unserialize") - 1))) {
      orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
  } else {
    if (config_unserialize->dump) {
      sp_log_request(config_unserialize->dump,
                     config_unserialize->textual_representation,
                     SP_TOKEN_UNSERIALIZE_HMAC);
    }
    if (true == config_unserialize->simulation) {
      sp_log_msg("unserialize", SP_LOG_SIMULATION, "Invalid HMAC for %s",
                 serialized_str);
      if ((orig_handler = zend_hash_str_find_ptr(
               SNUFFLEUPAGUS_G(sp_internal_functions_hook), "unserialize",
               sizeof("unserialize") - 1))) {
        orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
      }
    } else {
      sp_log_msg("unserialize", SP_LOG_DROP, "Invalid HMAC for %s",
                 serialized_str);
    }
  }
  efree(serialized_str);
  return;
}

int hook_serialize(void) {
  TSRMLS_FETCH();

  HOOK_FUNCTION("serialize", sp_internal_functions_hook, PHP_FN(sp_serialize));
  HOOK_FUNCTION("unserialize", sp_internal_functions_hook,
                PHP_FN(sp_unserialize));

  return SUCCESS;
}
