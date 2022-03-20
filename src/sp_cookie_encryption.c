#include "php_snuffleupagus.h"

static inline const sp_cookie *sp_lookup_cookie_config(const zend_string *key) {
  const sp_list_node *it = SPCFG(cookie).cookies;

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
  const sp_cookie *cookie = sp_lookup_cookie_config(hash_key->key);

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

static zend_string *encrypt_data(zend_string *data) {
  zend_string *z = encrypt_zval(data);
  sp_log_debug("Cookie value:%s:", ZSTR_VAL(z));
  return z;
}

#if PHP_VERSION_ID >= 70300
static void php_head_parse_cookie_options_array(
    zval *options, zend_long *expires, zend_string **path, zend_string **domain,
    zend_bool *secure, zend_bool *httponly, zend_string **samesite) {
  int found = 0;
  zend_string *key;
  zval *value;

  ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(options), key, value) {
    if (key) {
      if (zend_string_equals_literal_ci(key, "expires")) {
        *expires = zval_get_long(value);
        found++;
      } else if (zend_string_equals_literal_ci(key, "path")) {
        *path = zval_get_string(value);
        found++;
      } else if (zend_string_equals_literal_ci(key, "domain")) {
        *domain = zval_get_string(value);
        found++;
      } else if (zend_string_equals_literal_ci(key, "secure")) {
        *secure = zval_is_true(value);
        found++;
      } else if (zend_string_equals_literal_ci(key, "httponly")) {
        *httponly = zval_is_true(value);
        found++;
      } else if (zend_string_equals_literal_ci(key, "samesite")) {
        *samesite = zval_get_string(value);
        found++;
      } else {
        php_error_docref(NULL, E_WARNING,
                         "Unrecognized key '%s' found in the options array",
                         ZSTR_VAL(key));
      }
    } else {
      php_error_docref(NULL, E_WARNING,
                       "Numeric key found in the options array");
    }
  }
  ZEND_HASH_FOREACH_END();

  /* Array is not empty but no valid keys were found */
  if (found == 0 && zend_hash_num_elements(Z_ARRVAL_P(options)) > 0) {
    php_error_docref(NULL, E_WARNING,
                     "No valid options were found in the given array");
  }
}
#endif

PHP_FUNCTION(sp_setcookie) {
  zend_string *name = NULL, *value = NULL, *path = NULL, *domain = NULL,
              *value_enc = NULL,
#if PHP_VERSION_ID < 70300
              *path_samesite = NULL;
#else
              *samesite = NULL;
#endif

  zend_long expires = 0;
  zval *expires_or_options = NULL;
  zend_bool secure = 0, httponly = 0;
  const sp_cookie *cookie_node = NULL;
  char *cookie_samesite;

  // LCOV_EXCL_BR_START
  ZEND_PARSE_PARAMETERS_START(1, 7)
  Z_PARAM_STR(name)
  Z_PARAM_OPTIONAL
  Z_PARAM_STR(value)
  Z_PARAM_ZVAL(expires_or_options)
  Z_PARAM_STR(path)
  Z_PARAM_STR(domain)
  Z_PARAM_BOOL(secure)
  Z_PARAM_BOOL(httponly)
  ZEND_PARSE_PARAMETERS_END();
  // LCOV_EXCL_BR_END

  if (expires_or_options) {
#if PHP_VERSION_ID < 70300
    expires = zval_get_long(expires_or_options);
#else
    if (Z_TYPE_P(expires_or_options) == IS_ARRAY) {
      if (UNEXPECTED(ZEND_NUM_ARGS() > 3)) {
        php_error_docref(NULL, E_WARNING,
                         "Cannot pass arguments after the options array");
        RETURN_FALSE;
      }
      php_head_parse_cookie_options_array(expires_or_options, &expires, &path,
                                          &domain, &secure, &httponly,
                                          &samesite);
    } else {
      expires = zval_get_long(expires_or_options);
    }
#endif
  }

  /* If the request was issued over HTTPS, the cookie should be "secure" */
  if (SPCFG(auto_cookie_secure).enable) {
    const zval server_vars = PG(http_globals)[TRACK_VARS_SERVER];
    if (Z_TYPE(server_vars) == IS_ARRAY) {
      const zval *is_https =
          zend_hash_str_find(Z_ARRVAL(server_vars), ZEND_STRL("HTTPS"));
      if (NULL != is_https) {
        secure = 1;
      }
    }
  }

  /* lookup existing configuration for said cookie */
  cookie_node = sp_lookup_cookie_config(name);

  /* If the cookie's value is encrypted, it won't be usable by
   * javascript anyway.
   */
  if (cookie_node && cookie_node->encrypt) {
    httponly = 1;
  }

  /* Shall we encrypt the cookie's value? */
  if (cookie_node && cookie_node->encrypt && value) {
    value_enc = encrypt_data(value);
  }

  if (cookie_node && cookie_node->samesite) {
    if (!path) {
      path = zend_string_init("", 0, 0);
    }
#if PHP_VERSION_ID < 70300
    cookie_samesite = (cookie_node->samesite == lax)
                          ? SAMESITE_COOKIE_FORMAT SP_TOKEN_SAMESITE_LAX
                          : SAMESITE_COOKIE_FORMAT SP_TOKEN_SAMESITE_STRICT;

    /* Concatenating everything, as is in PHP internals */
    path_samesite = zend_string_init(ZSTR_VAL(path), ZSTR_LEN(path), 0);
    path_samesite = zend_string_extend(
        path_samesite, ZSTR_LEN(path) + strlen(cookie_samesite) + 1, 0);
    memcpy(ZSTR_VAL(path_samesite) + ZSTR_LEN(path), cookie_samesite,
           strlen(cookie_samesite) + 1);
#else
    cookie_samesite = (cookie_node->samesite == lax) ? SP_TOKEN_SAMESITE_LAX
                                                     : SP_TOKEN_SAMESITE_STRICT;

    samesite = zend_string_init(cookie_samesite, strlen(cookie_samesite), 0);
#endif
  }

#if PHP_VERSION_ID < 70300
  if (php_setcookie(name, (value_enc ? value_enc : value), expires,
                    (path_samesite ? path_samesite : path), domain, secure, 1,
                    httponly) == SUCCESS) {
#else
  if (php_setcookie(name, (value_enc ? value_enc : value), expires, path,
                    domain, secure, httponly, samesite, 1) == SUCCESS) {
#endif
    RETVAL_TRUE;
  } else {
    RETVAL_FALSE;
  }

  if (value_enc) {
    zend_string_release(value_enc);
  }
#if PHP_VERSION_ID < 70300
  if (path_samesite) {
    zend_string_release(path_samesite);
  }
#endif
}

int hook_cookies() {
  HOOK_FUNCTION("setcookie", sp_internal_functions_hook, PHP_FN(sp_setcookie));

  return SUCCESS;
}
