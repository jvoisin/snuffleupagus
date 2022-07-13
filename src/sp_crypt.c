#include "php_snuffleupagus.h"

void generate_key(unsigned char *key) {
  PHP_SHA256_CTX ctx;
  const char *user_agent = getenv("HTTP_USER_AGENT");
  const zend_string *env_var_zend = SPCFG(cookies_env_var);
  const zend_string *encryption_key_zend = SPCFG(encryption_key);
  const char *env_var = (env_var_zend ? getenv(ZSTR_VAL(env_var_zend)) : NULL);
  const char *encryption_key = (encryption_key_zend ? ZSTR_VAL(encryption_key_zend) : NULL);

  assert(32 == crypto_secretbox_KEYBYTES);  // 32 is the size of a SHA256.
  assert(encryption_key);                   // Encryption key can't be NULL

  PHP_SHA256Init(&ctx);

  if (user_agent) {
    PHP_SHA256Update(&ctx, (unsigned char *)user_agent, strlen(user_agent));
  }

  if (env_var) {
    PHP_SHA256Update(&ctx, (unsigned char *)env_var, strlen(env_var));
  } else {
    sp_log_warn("cookie_encryption",
                "The environment variable '%s' "
                "is empty, cookies are weakly encrypted",
                ZSTR_VAL(env_var_zend));
  }

  if (encryption_key) {
    PHP_SHA256Update(&ctx, (const unsigned char *)encryption_key,
                     strlen(encryption_key));
  }

  PHP_SHA256Final((unsigned char *)key, &ctx);
}

// This function return 0 upon success , non-zero otherwise
int decrypt_zval(zval *pDest, bool simulation, zend_hash_key *hash_key) {
  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  unsigned char *decrypted = NULL, *backup = NULL;
  int ret = 0;

  zend_string *debase64 = php_base64_decode((unsigned char *)(Z_STRVAL_P(pDest)), Z_STRLEN_P(pDest));

  if (ZSTR_LEN(debase64) < crypto_secretbox_NONCEBYTES) {
    if (true == simulation) {
      sp_log_simulation(
          "cookie_encryption",
          "Buffer underflow tentative detected in cookie encryption handling "
          "for %s. Using the cookie 'as is' instead of decrypting it",
          hash_key ? ZSTR_VAL(hash_key->key) : "the session");
      ret = ZEND_HASH_APPLY_KEEP; goto out;
    } else {
      // LCOV_EXCL_START
      sp_log_drop(
          "cookie_encryption",
          "Buffer underflow (tentative) detected in cookie encryption handling");
      ret = ZEND_HASH_APPLY_REMOVE; goto out;
      // LCOV_EXCL_STOP
    }
  }

  // LCOV_EXCL_START
  if (ZSTR_LEN(debase64) + (size_t)crypto_secretbox_ZEROBYTES <
      ZSTR_LEN(debase64)) {
    if (true == simulation) {
      sp_log_simulation(
          "cookie_encryption",
          "Integer overflow (tentative) detected in cookie encryption handling "
          "for %s. Using the cookie 'as it' instead of decrypting it.",
          hash_key ? ZSTR_VAL(hash_key->key) : "the session");
      ret = ZEND_HASH_APPLY_KEEP; goto out;
    } else {
      sp_log_drop(
          "cookie_encryption",
          "Integer overflow (tentative) detected in cookie encryption handling.");
      ret = ZEND_HASH_APPLY_REMOVE; goto out;
    }
  }
  // LCOV_EXCL_STOP

  generate_key(key);

  decrypted = ecalloc(ZSTR_LEN(debase64) + crypto_secretbox_ZEROBYTES, 1);
  backup = ecalloc(ZSTR_LEN(debase64), 1);
  memcpy(backup, ZSTR_VAL(debase64), ZSTR_LEN(debase64));

  ret = crypto_secretbox_open(
      decrypted,
      (unsigned char *)(ZSTR_VAL(debase64) + crypto_secretbox_NONCEBYTES),
      ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES,
      (unsigned char *)ZSTR_VAL(debase64), key);

  if (-1 == ret) {
    if (true == simulation) {
      sp_log_simulation(
          "cookie_encryption",
          "Something went wrong with the decryption of %s. Using the cookie "
          "'as is' instead of decrypting it",
          hash_key ? ZSTR_VAL(hash_key->key) : "the session");
      memcpy(ZSTR_VAL(debase64), backup, ZSTR_LEN(debase64));
      ret = ZEND_HASH_APPLY_KEEP; goto out;
    } else {
      sp_log_warn("cookie_encryption",
                  "Something went wrong with the decryption of %s",
                  hash_key ? ZSTR_VAL(hash_key->key) : "the session");
      ret = ZEND_HASH_APPLY_REMOVE; goto out;
    }
  }

  ZVAL_STRINGL(pDest, (char *)(decrypted + crypto_secretbox_ZEROBYTES),
               ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES - 1 -
                   crypto_secretbox_ZEROBYTES);

  ret = ZEND_HASH_APPLY_KEEP;

out:
  zend_string_efree(debase64);
  efree(decrypted);
  efree(backup);

  return ret;
}

/*
** This function will return the `data` of length `data_len` encrypted in the
** form `base64(nonce | encrypted_data)` (with `|` being the concatenation
** operation).
*/
zend_string *encrypt_zval(zend_string *data) {
  const size_t encrypted_msg_len =
      crypto_secretbox_ZEROBYTES + ZSTR_LEN(data) + 1;
  // FIXME : We know that this len is too long
  const size_t emsg_and_nonce_len =
      encrypted_msg_len + crypto_secretbox_NONCEBYTES;

  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  unsigned char nonce[crypto_secretbox_NONCEBYTES] = {0};
  unsigned char *data_to_encrypt = ecalloc(encrypted_msg_len, 1);
  unsigned char *encrypted_data = ecalloc(emsg_and_nonce_len, 1);

  generate_key(key);

  // Put random bytes in the nonce
  php_random_bytes(nonce, sizeof(nonce), 0);

  /* tweetnacl's API requires the message to be padded with
  crypto_secretbox_ZEROBYTES zeroes. */
  memcpy(data_to_encrypt + crypto_secretbox_ZEROBYTES, ZSTR_VAL(data),
         ZSTR_LEN(data));

  assert(sizeof(zend_long) <= crypto_secretbox_NONCEBYTES);

  memcpy(encrypted_data, nonce, crypto_secretbox_NONCEBYTES);

  int err = crypto_secretbox(encrypted_data + crypto_secretbox_NONCEBYTES,
                   data_to_encrypt, encrypted_msg_len, nonce, key);

  zend_string *z = NULL;
  if (err) {
    sp_log_err("cookie_encryption", "something went wrong during encryption");
    z = zend_string_init("<sp_encryption_error>", 21, 0);
  } else {
    z = php_base64_encode(encrypted_data, emsg_and_nonce_len);
  }

  efree(data_to_encrypt);
  efree(encrypted_data);

  return z;
}
