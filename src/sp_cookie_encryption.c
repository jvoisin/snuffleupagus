#include "php_snuffleupagus.h"

#include "ext/standard/url.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

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

  return decrypt_zval(pDest, cookie->simulation, hash_key);
}

/*
** This function will return the `data` of length `data_len` encrypted in the
** form `base64(nonce | encrypted_data)` (with `|` being the concatenation
** operation).
*/
static zend_string *encrypt_data(char *data, unsigned long long data_len) {
  zend_string *z = encrypt_zval(data, data_len);
  sp_log_debug("cookie_encryption", "Cookie value:%s:", z->val);
  return z;
}

PHP_FUNCTION(sp_setcookie) {
  zval params[7] = {{{0}}};
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
  HOOK_FUNCTION("setcookie", sp_internal_functions_hook, PHP_FN(sp_setcookie));

  return SUCCESS;
}
