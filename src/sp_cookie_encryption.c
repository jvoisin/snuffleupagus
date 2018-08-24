#include "php_snuffleupagus.h"

#include "ext/standard/url.h"
#include "ext/standard/head.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static inline const sp_cookie *sp_lookup_cookie_config(const zend_string *key) {
  const sp_list_node *it = SNUFFLEUPAGUS_G(config).config_cookie->cookies;

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
  sp_log_debug("cookie_encryption", "Cookie value:%s:", ZSTR_VAL(z));
  return z;
}

PHP_FUNCTION(sp_setcookie) {
  zend_string *name = NULL, *value = NULL, *path = NULL, *domain = NULL, *value_enc = NULL,
#if PHP_VERSION_ID < 70300
              *path_samesite = NULL;
#else
              *samesite = NULL;
#endif

  zend_long expires = 0;
  zend_bool secure = 0, httponly = 0;
  const sp_cookie *cookie_node = NULL;
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
    cookie_samesite = (cookie_node->samesite == lax)
                          ? SP_TOKEN_SAMESITE_LAX
                          : SP_TOKEN_SAMESITE_STRICT;

    samesite = zend_string_init(cookie_samesite, strlen(cookie_samesite), 0);
#endif
  }


#if PHP_VERSION_ID < 70300
  if (php_setcookie(name, (value_enc ? value_enc : value), expires, (path_samesite ? path_samesite : path), domain, secure, 1, httponly)) {
#else
  if (php_setcookie(name, (value_enc ? value_enc : value), expires, path, domain, secure, httponly, samesite, 1)) {
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
  RETURN_TRUE; // TODO why always true ?
}

int hook_cookies() {
  HOOK_FUNCTION("setcookie", sp_internal_functions_hook, PHP_FN(sp_setcookie));

  return SUCCESS;
}
