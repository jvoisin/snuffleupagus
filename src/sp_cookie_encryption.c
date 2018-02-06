#include "php_snuffleupagus.h"

#include "ext/standard/url.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static zend_long nonce_d = 0;

static inline void generate_key(unsigned char *key) {
  PHP_SHA256_CTX ctx;
  const char *user_agent = getenv("HTTP_USER_AGENT");
  const char *env_var =
      getenv(SNUFFLEUPAGUS_G(config).config_snuffleupagus->cookies_env_var);
  const char *encryption_key =
      SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key;

  assert(32 == crypto_secretbox_KEYBYTES);  // 32 is the size of a SHA256.
  assert(encryption_key);                   // Encryption key can't be NULL

  PHP_SHA256Init(&ctx);

  if (user_agent) {
    PHP_SHA256Update(&ctx, (unsigned char *)user_agent, strlen(user_agent));
  }

  if (env_var) {
    PHP_SHA256Update(&ctx, (unsigned char *)env_var, strlen(env_var));
  } else {
    sp_log_err("cookie_encryption",
               "The environment variable '%s'"
               "is empty, cookies are weakly encrypted.",
               SNUFFLEUPAGUS_G(config).config_snuffleupagus->cookies_env_var);
  }

  if (encryption_key) {
    PHP_SHA256Update(&ctx, (const unsigned char *)encryption_key,
                     strlen(encryption_key));
  }

  PHP_SHA256Final((unsigned char *)key, &ctx);
}

static inline const sp_cookie *sp_lookup_cookie_config(const char *key) {
  sp_list_node *it = SNUFFLEUPAGUS_G(config).config_cookie->cookies;

  while (it) {
    const sp_cookie *config = it->data;
    if (config && sp_match_value(key, config->name, config->name_r)) {
      return config;
    }
    it = it->next;
  }
  return NULL;
}

/* called at RINIT time with each cookie, eventually decrypt said cookie */
int decrypt_cookie(zval *pDest, int num_args, va_list args,
                   zend_hash_key *hash_key) {
  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  zend_string *debase64;
  unsigned char *decrypted;
  const sp_cookie *cookie = sp_lookup_cookie_config(ZSTR_VAL(hash_key->key));
  int ret = 0;

  /* If the cookie isn't in the conf, it shouldn't be encrypted. */
  if (!cookie || !cookie->encrypt) {
    return ZEND_HASH_APPLY_KEEP;
  }

  /* If the cookie has no value, it shouldn't be encrypted. */
  if (0 == Z_STRLEN_P(pDest)) {
    return ZEND_HASH_APPLY_KEEP;
  }

  debase64 = php_base64_decode((unsigned char *)(Z_STRVAL_P(pDest)),
                               Z_STRLEN_P(pDest));

  if (ZSTR_LEN(debase64) <
      crypto_secretbox_NONCEBYTES + crypto_secretbox_ZEROBYTES) {
    if (true == cookie->simulation) {
      sp_log_msg(
          "cookie_encryption", SP_LOG_SIMULATION,
          "Buffer underflow tentative detected in cookie encryption handling "
          "for %s. Using the cookie 'as it' instead of decrypting it.",
          ZSTR_VAL(hash_key->key));
      return ZEND_HASH_APPLY_KEEP;
    } else {
      sp_log_msg(
          "cookie_encryption", SP_LOG_DROP,
          "Buffer underflow tentative detected in cookie encryption handling.");
      return ZEND_HASH_APPLY_REMOVE;
    }
  }

  generate_key(key);

  decrypted = ecalloc(ZSTR_LEN(debase64), 1);

  ret = crypto_secretbox_open(
      decrypted,
      (unsigned char *)(ZSTR_VAL(debase64) + crypto_secretbox_NONCEBYTES),
      ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES,
      (unsigned char *)ZSTR_VAL(debase64), key);

  if (-1 == ret) {
    if (true == cookie->simulation) {
      sp_log_msg(
          "cookie_encryption", SP_LOG_SIMULATION,
          "Something went wrong with the decryption of %s. Using the cookie "
          "'as it' instead of decrypting it",
          ZSTR_VAL(hash_key->key));
      return ZEND_HASH_APPLY_KEEP;
    } else {
      sp_log_msg("cookie_encryption", SP_LOG_DROP,
                 "Something went wrong with the decryption of %s.",
                 ZSTR_VAL(hash_key->key));
      return ZEND_HASH_APPLY_REMOVE;
    }
  }

  ZVAL_STRINGL(pDest, (char *)(decrypted + crypto_secretbox_ZEROBYTES),
               ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES - 1 -
                   crypto_secretbox_ZEROBYTES);

  return ZEND_HASH_APPLY_KEEP;
}

/*
** This function will return the `data` of length `data_len` encrypted in the
** form `base64(nonce | encrypted_data)` (with `|` being the concatenation
** operation).
*/
static zend_string *encrypt_data(char *data, unsigned long long data_len) {
  const size_t encrypted_msg_len = crypto_secretbox_ZEROBYTES + data_len + 1;
  const size_t emsg_and_nonce_len =
      encrypted_msg_len + crypto_secretbox_NONCEBYTES;

  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  unsigned char nonce[crypto_secretbox_NONCEBYTES] = {0};
  unsigned char *data_to_encrypt = ecalloc(encrypted_msg_len, 1);
  unsigned char *encrypted_data = ecalloc(emsg_and_nonce_len, 1);

  generate_key(key);

  /* tweetnacl's API requires the message to be padded with
  crypto_secretbox_ZEROBYTES zeroes. */
  memcpy(data_to_encrypt + crypto_secretbox_ZEROBYTES, data, data_len);

  assert(sizeof(zend_long) <= crypto_secretbox_NONCEBYTES);

  if (0 == nonce_d) {
    /* A zend_long should be enough to avoid collisions */
    if (php_random_int_throw(0, ZEND_LONG_MAX, &nonce_d) == FAILURE) {
      return NULL;  // LCOV_EXCL_LINE
    }
  }
  nonce_d++;
  sscanf((char *)nonce, "%ld", &nonce_d);

  memcpy(encrypted_data, nonce, crypto_secretbox_NONCEBYTES);
  crypto_secretbox(encrypted_data + crypto_secretbox_NONCEBYTES,
                   data_to_encrypt, encrypted_msg_len, nonce, key);

  zend_string *z = php_base64_encode(encrypted_data, emsg_and_nonce_len);
  sp_log_debug("cookie_encryption", "Cookie value:%s:", z->val);
  return z;
}

PHP_FUNCTION(sp_setcookie) {
  zval params[7] = {0};
  zend_string *name = NULL, *value = NULL, *path = NULL, *domain = NULL,
              *samesite = NULL;
  zend_long expires = 0;
  zend_bool secure = 0, httponly = 0;
  const sp_cookie *cookie_node = NULL;
  zval func_name;
  char *cookie_samesite;

  // LCOV_EXCL_BR_START
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
  // LCOV_EXCL_BR_END

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

  /* lookup existing configuration for said cookie */
  cookie_node = sp_lookup_cookie_config(ZSTR_VAL(name));

  /* If the cookie's value is encrypted, it won't be usable by
   * javascript anyway.
   */
  if (cookie_node && cookie_node->encrypt) {
    httponly = 1;
  }

  /* Shall we encrypt the cookie's value? */
  if (cookie_node && cookie_node->encrypt && value) {
    zend_string *encrypted_data = encrypt_data(value->val, value->len);
    ZVAL_STR_COPY(&params[1], encrypted_data);
    zend_string_release(encrypted_data);
  } else if (value) {
    ZVAL_STR_COPY(&params[1], value);
  }

  ZVAL_STRING(&func_name, "setcookie");
  ZVAL_STR_COPY(&params[0], name);
  ZVAL_LONG(&params[2], expires);
  if (domain) {
    ZVAL_STR_COPY(&params[4], domain);
  }
  if (secure) {
    ZVAL_LONG(&params[5], secure);
  }
  if (httponly) {
    ZVAL_LONG(&params[6], httponly);
  }

  /* param[3](path) is concatenated to path= and is not filtered, we can inject
  the samesite parameter here */
  if (cookie_node && cookie_node->samesite) {
    if (!path) {
      path = zend_string_init("", 0, 0);
    }
    cookie_samesite = (cookie_node->samesite == lax)
                          ? SAMESITE_COOKIE_FORMAT SP_TOKEN_SAMESITE_LAX
                          : SAMESITE_COOKIE_FORMAT SP_TOKEN_SAMESITE_STRICT;
    /* Concatenating everything, as is in PHP internals */
    samesite = zend_string_init(ZSTR_VAL(path), ZSTR_LEN(path), 0);
    samesite = zend_string_extend(
        samesite, ZSTR_LEN(path) + strlen(cookie_samesite) + 1, 0);
    memcpy(ZSTR_VAL(samesite) + ZSTR_LEN(path), cookie_samesite,
           strlen(cookie_samesite) + 1);
    ZVAL_STR_COPY(&params[3], samesite);
  } else if (path) {
    ZVAL_STR_COPY(&params[3], path);
  }

  /* This is the _fun_ part: because PHP is utterly idiotic and nonsensical,
  the `call_user_function` macro will __discard__ (yes) its first argument
  (the hashtable), effectively calling functions from `CG(function_table)`.
  This is why were replacing our hook with the original function, calling
  the function, and then re-hooking it. */
  void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
  handler = zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(sp_internal_functions_hook),
                                   "setcookie", strlen("setcookie"));
  zend_internal_function *func = zend_hash_str_find_ptr(
      CG(function_table), "setcookie", strlen("setcookie"));
  func->handler = handler;

  call_user_function(CG(function_table), NULL, &func_name, return_value, 7,
                     params);

  func->handler = PHP_FN(sp_setcookie);
  RETURN_TRUE;
}

int hook_cookies() {
  HOOK_FUNCTION("setcookie", sp_internal_functions_hook, PHP_FN(sp_setcookie),
                false);

  return SUCCESS;
}
