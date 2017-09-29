#include "php_snuffleupagus.h"

#include "ext/standard/url.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static unsigned int nonce_d = 0;

static inline void generate_key(unsigned char *key) {
  PHP_SHA256_CTX ctx;
  const char *user_agent = sp_getenv("HTTP_USER_AGENT");
  const char *remote_addr = sp_getenv("REMOTE_ADDR");
  const char *encryption_key =
      SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key;

  /* 32 is the size of a SHA256. */
  assert(32 == crypto_secretbox_KEYBYTES);

  PHP_SHA256Init(&ctx);

  if (user_agent) {
    PHP_SHA256Update(&ctx, (unsigned char *)user_agent, strlen(user_agent));
  }

  if (remote_addr) {
    char out[128];
	  apply_mask_on_ip(out, remote_addr);
    PHP_SHA256Update(&ctx, (unsigned char*)out, sizeof(out));
  }

  if (encryption_key) {
    PHP_SHA256Update(&ctx, (const unsigned char *)encryption_key,
                     strlen(encryption_key));
  }

  PHP_SHA256Final((unsigned char *)key, &ctx);
}

int decrypt_cookie(zval *pDest, int num_args, va_list args,
                   zend_hash_key *hash_key) {
  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  size_t value_len;
  zend_string *debase64;
  unsigned char *decrypted;
  int ret = 0;

  /* If the cookie isn't in the conf, it shouldn't be encrypted. */
  if (0 ==
      zend_hash_exists(SNUFFLEUPAGUS_G(config).config_cookie_encryption->names,
                       hash_key->key)) {
    return ZEND_HASH_APPLY_KEEP;
  }

  generate_key(key);

  value_len = php_url_decode(Z_STRVAL_P(pDest), Z_STRLEN_P(pDest));

  if (value_len == 0) {
    return ZEND_HASH_APPLY_KEEP;
  }

  debase64 = php_base64_decode((unsigned char *)(Z_STRVAL_P(pDest)), value_len);

  if (ZSTR_LEN(debase64) <
      crypto_secretbox_NONCEBYTES + crypto_secretbox_ZEROBYTES) {
    sp_log_msg("cookie_encryption", SP_LOG_DROP,
        "Buffer underflow tentative detected in cookie encryption handling.");
    return ZEND_HASH_APPLY_REMOVE;
  }

  decrypted = pecalloc(ZSTR_LEN(debase64), 1, 0);

  ret = crypto_secretbox_open(
      decrypted,
      (unsigned char *)(ZSTR_VAL(debase64) + crypto_secretbox_NONCEBYTES),
      ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES,
      (unsigned char *)ZSTR_VAL(debase64), key);

  if (ret == -1) {
    sp_log_msg("cookie_encryption", SP_LOG_DROP,
      "Something went wrong with the decryption of %s.",
               ZSTR_VAL(hash_key->key));
    return ZEND_HASH_APPLY_REMOVE;
  }

  ZVAL_STRINGL(pDest, (char *)(decrypted + crypto_secretbox_ZEROBYTES),
               ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES - 1 -
                   crypto_secretbox_ZEROBYTES);

  return ZEND_HASH_APPLY_KEEP;
}

/**
  This function will return the `data` of length `data_len` encrypted in the
  form
  base64(nonce | encrypted_data) (with `|` being the concatenation
  operation).

  The `nonce` is time-based.
*/
static zend_string *encrypt_data(char *data, unsigned long long data_len) {
  const size_t encrypted_msg_len = crypto_secretbox_ZEROBYTES + data_len + 1;
  const size_t emsg_and_nonce_len = encrypted_msg_len + crypto_secretbox_NONCEBYTES;

  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  unsigned char nonce[crypto_secretbox_NONCEBYTES] = {0};
  unsigned char *data_to_encrypt = pecalloc(encrypted_msg_len, 1, 0);
  unsigned char *encrypted_data = pecalloc(emsg_and_nonce_len, 1, 1);

  generate_key(key);

  /* tweetnacl's API requires the message to be padded with
  crypto_secretbox_ZEROBYTES zeroes. */
  memcpy(data_to_encrypt + crypto_secretbox_ZEROBYTES, data, data_len);

  assert(sizeof(size_t) <= crypto_secretbox_NONCEBYTES);

  nonce_d++;
  sscanf((char*)nonce, "%ud", &nonce_d); 

  memcpy(encrypted_data, nonce, crypto_secretbox_NONCEBYTES);
  crypto_secretbox(encrypted_data + crypto_secretbox_NONCEBYTES,
                   data_to_encrypt, encrypted_msg_len, nonce, key);

  zend_string *z = php_base64_encode(encrypted_data, emsg_and_nonce_len);
  sp_log_debug("cookie_encryption", "Cookie value:%s:", z->val);
  return z;
}

PHP_FUNCTION(sp_setcookie) {
  zval params[7] = { 0 };
  zend_string *name = NULL, *value = NULL, *path = NULL, *domain = NULL;
  zend_long expires = 0;
  zend_bool secure = 0, httponly = 0;
  zval ret_val;
  zval func_name;

  ZEND_PARSE_PARAMETERS_START(1, 7)
  Z_PARAM_STR(name)
  Z_PARAM_OPTIONAL
  Z_PARAM_STR(value)
  Z_PARAM_LONG(expires)
  Z_PARAM_STR(path)
  Z_PARAM_STR(domain)
  Z_PARAM_BOOL(secure)
  Z_PARAM_BOOL(httponly)
  ZEND_PARSE_PARAMETERS_END();

  /* If the request was issued over HTTPS, the cookie should be "secure" */
  if (SNUFFLEUPAGUS_G(config).config_auto_cookie_secure) {
    const zval server_vars = PG(http_globals)[TRACK_VARS_SERVER];
    if (Z_TYPE(server_vars) == IS_ARRAY) {
      const zval *is_https =
          zend_hash_str_find(Z_ARRVAL(server_vars), "HTTPS", strlen("HTTPS"));
      if (NULL != is_https) {
        secure = 1;
      }
    }
  }

  /* If the cookie's value is encrypted, it won't be usable by
   * javascript anyway.
   */
  if (zend_hash_exists(SNUFFLEUPAGUS_G(config).config_cookie_encryption->names,
                       name) > 0) {
    httponly = 1;
  }

  /* Shall we encrypt the cookie's value? */
  if (zend_hash_exists(SNUFFLEUPAGUS_G(config).config_cookie_encryption->names,
                       name) > 0 && value) {
    zend_string *encrypted_data = encrypt_data(value->val, value->len);
    ZVAL_STR_COPY(&params[1], encrypted_data);
    zend_string_release(encrypted_data);
  } else if (value) {
    ZVAL_STR_COPY(&params[1], value);
  }

  ZVAL_STRING(&func_name, "setcookie");
  ZVAL_STR_COPY(&params[0], name);
  ZVAL_LONG(&params[2], expires);
  if (path) {
    ZVAL_STR_COPY(&params[3], path);
  }
  if (domain) {
    ZVAL_STR_COPY(&params[4], domain);
  }
  if (secure) {
    ZVAL_LONG(&params[5], secure);
  }
  if (httponly) {
    ZVAL_LONG(&params[6], httponly);
  }

  /* This is the _fun_ part: because PHP is utterly idiotic and nonsensical,
  the `call_user_function` macro will __discard__ (yes) its first argument
  (the hashtable), effectively calling functions from `CG(function_table)`.
  This is why were replacing our hook with the original function, calling
  the function, and then re-hooking it. */
  void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
  handler = zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(sp_internal_functions_hook), "setcookie",
                                   strlen("setcookie"));
  zend_internal_function *func = zend_hash_str_find_ptr(
      CG(function_table), "setcookie", strlen("setcookie"));
  func->handler = handler;

  call_user_function(CG(function_table), NULL, &func_name, &ret_val, 7, params);

  func->handler = PHP_FN(sp_setcookie);
}

int hook_cookies() {
  HOOK_FUNCTION("setcookie", sp_internal_functions_hook, PHP_FN(sp_setcookie), false);

  return SUCCESS;
}
