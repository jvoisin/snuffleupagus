#include "php_snuffleupagus.h"

// condensed version of PHP's php_hash_do_hash_hmac() in ext/hash/hash.c
#if PHP_VERSION_ID < 80000
static inline void *php_hash_alloc_context(const php_hash_ops *ops) {
  /* Zero out context memory so serialization doesn't expose internals */
  return ecalloc(1, ops->context_size);
}
#endif

static zend_string *sp_do_hash_hmac_sha256(char* restrict data, size_t data_len, char* restrict key, size_t key_len)
{
#if PHP_VERSION_ID < 80000
  const php_hash_ops *ops = php_hash_fetch_ops(ZEND_STRL("sha256"));
#else
  zend_string *algo = zend_string_init(ZEND_STRL("sha256"), 0);
  const php_hash_ops *ops = php_hash_fetch_ops(algo);
  zend_string_release_ex(algo, 0);
#endif

#if PHP_VERSION_ID < 70200
  if (!ops) {
#else
  if (!ops || !ops->is_crypto) {
#endif
    sp_log_err("hmac", "unsupported hash algorithm: sha256");
    return NULL;
  }

  void *context = php_hash_alloc_context(ops);

  unsigned char *K = emalloc(ops->block_size);
  zend_string *digest = zend_string_alloc(ops->digest_size, 0);

  php_hash_hmac_prep_key(K, ops, context, (unsigned char *) key, key_len);
  php_hash_hmac_round((unsigned char *) ZSTR_VAL(digest), ops, context, K, (unsigned char *) data, data_len);
  php_hash_string_xor_char(K, K, 0x6A, ops->block_size);
  php_hash_hmac_round((unsigned char *) ZSTR_VAL(digest), ops, context, K, (unsigned char *) ZSTR_VAL(digest), ops->digest_size);

  /* Zero the key */
  ZEND_SECURE_ZERO(K, ops->block_size);
  efree(K);
  efree(context);

  zend_string *hex_digest = zend_string_safe_alloc(ops->digest_size, 2, 0, 0);

  php_hash_bin2hex(ZSTR_VAL(hex_digest), (unsigned char *) ZSTR_VAL(digest), ops->digest_size);
  ZSTR_VAL(hex_digest)[2 * ops->digest_size] = 0;
  zend_string_release_ex(digest, 0);
  return hex_digest;
}

PHP_FUNCTION(sp_serialize) {
  zif_handler orig_handler;

  /* Call the original `serialize` function. */
  orig_handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("serialize"));
  if (orig_handler) {
    orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  }

  if (!SPCFG(unserialize).enable) {
    return;
  }

  /* Compute the HMAC of the textual representation of the serialized data*/
  zend_string *hmac = sp_do_hash_hmac_sha256(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value), ZSTR_VAL(SPCFG(encryption_key)), ZSTR_LEN(SPCFG(encryption_key)));

  if (!hmac) {
    zend_bailout();
  }

  size_t len = Z_STRLEN_P(return_value) + ZSTR_LEN(hmac);
  if (len < Z_STRLEN_P(return_value)) {
    // LCOV_EXCL_START
    sp_log_err("overflow_error",
               "Overflow tentative detected in sp_serialize.");
    zend_bailout();
    // LCOV_EXCL_STOP
  }

  /* Append the computed HMAC to the serialized data. */
  zend_string *orig_ret_str = return_value->value.str;
  RETVAL_NEW_STR(zend_string_concat2(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value), ZSTR_VAL(hmac), ZSTR_LEN(hmac)));
  zend_string_free(orig_ret_str);
}

PHP_FUNCTION(sp_unserialize) {
  char *buf = NULL;
  size_t buf_len = 0;
  HashTable *opts = NULL;

  ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(buf, buf_len)
    Z_PARAM_OPTIONAL
    Z_PARAM_ARRAY_HT(opts)
  ZEND_PARSE_PARAMETERS_END();

  if (SPCFG(unserialize_noclass).enable) {
#if PHP_VERSION_ID > 80000
    HashTable ht;
    zend_hash_init(&ht, 1, NULL, NULL, 0);
    zval zv;
    ZVAL_FALSE(&zv);
    zend_hash_str_add(&ht, "allowed_classes", sizeof("allowed_classes")-1, &zv);
    php_unserialize_with_options(return_value, buf, buf_len, &ht, "unserialize");
    return;
#else
    sp_log_drop("unserialize_noclass", "unserialize_noclass is only supported on PHP8+");
#endif
  }

  /* 64 is the length of HMAC-256 */
  if (buf_len < 64) {
    sp_log_drop("unserialize", "The serialized object is too small.");
  }

  char* hmac = buf + buf_len - 64;
  char* serialized_str = ecalloc(buf_len - 64 + 1, 1);
  memcpy(serialized_str, buf, buf_len - 64);

  zend_string *expected_hmac = sp_do_hash_hmac_sha256(serialized_str, strlen(serialized_str), ZSTR_VAL(SPCFG(encryption_key)), ZSTR_LEN(SPCFG(encryption_key)));

  unsigned int status = 0;
  if (expected_hmac) {
    for (uint8_t i = 0; i < 64; i++) {
      status |= (hmac[i] ^ (ZSTR_VAL(expected_hmac))[i]);
    }
  } else { status = 1; }

  zif_handler orig_handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("unserialize"));
  if (0 == status) {
#if PHP_VERSION_ID >= 80300
      // PHP8.3 gives a warning about trailing data in unserialize strings.
      php_unserialize_with_options(return_value, buf, buf_len - 64, opts, "unserialize");
#else
    if ((orig_handler)) {
      orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
#endif
  } else {
    const sp_config_unserialize *config_unserialize = &(SPCFG(unserialize));
    if (config_unserialize->dump) {
      sp_log_request(config_unserialize->dump,
                     config_unserialize->textual_representation);
    }
    if (true == config_unserialize->simulation) {
      sp_log_simulation("unserialize", "Invalid HMAC for %s", serialized_str);
#if PHP_VERSION_ID >= 80300
      // PHP8.3 gives a warning about trailing data in unserialize strings.
      php_unserialize_with_options(return_value, buf, buf_len - 64, opts, "unserialize");
#else
      if ((orig_handler)) {
        orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
      }
#endif
    } else {
      sp_log_drop("unserialize", "Invalid HMAC for %s", serialized_str);
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
