#include "php_snuffleupagus.h"

#include "ext/standard/url.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static zend_long nonce_d = 0;

void generate_key(unsigned char *key) {
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

// This function return 0 upon success , non-zero otherwise
int decrypt_zval(zval *pDest, bool simulation, zend_hash_key *hash_key) {
  unsigned char key[crypto_secretbox_KEYBYTES] = {0};
  unsigned char *decrypted;
  zend_string *debase64;
  int ret = 0;

  debase64 = php_base64_decode((unsigned char *)(Z_STRVAL_P(pDest)),
                               Z_STRLEN_P(pDest));

  if (ZSTR_LEN(debase64) <
      crypto_secretbox_NONCEBYTES) {
    if (true == simulation) {
      sp_log_msg(
          "cookie_encryption", SP_LOG_SIMULATION,
          "Buffer underflow tentative detected in cookie encryption handling "
          "for %s. Using the cookie 'as it' instead of decrypting it.",
          ZSTR_VAL(hash_key->key) ? ZSTR_VAL(hash_key->key) : "PHPSESSID");
      return ZEND_HASH_APPLY_KEEP;
    } else {
      sp_log_msg(
          "cookie_encryption", SP_LOG_DROP,
          "Buffer underflow tentative detected in cookie encryption handling.");
      return ZEND_HASH_APPLY_REMOVE;

    }
  }

  generate_key(key);
  char * tmp = calloc(ZSTR_LEN(debase64) + crypto_secretbox_ZEROBYTES, 1);
  memcpy(ZSTR_VAL(debase64), tmp + crypto_secretbox_ZEROBYTES, ZSTR_LEN(debase64));
  decrypted = ecalloc(ZSTR_LEN(debase64) + crypto_secretbox_ZEROBYTES, 1);

  ret = crypto_secretbox_open(
      decrypted,
      (unsigned char *)(ZSTR_VAL(debase64) + crypto_secretbox_NONCEBYTES),
      ZSTR_LEN(debase64) - crypto_secretbox_NONCEBYTES,
      (unsigned char *)ZSTR_VAL(debase64), key);

  if (-1 == ret) {
    if (true == simulation) {
      sp_log_msg(
          "cookie_encryption", SP_LOG_SIMULATION,
          "Something went wrong with the decryption of %s. Using the cookie "
          "'as it' instead of decrypting it",
          ZSTR_VAL(hash_key->key) ? ZSTR_VAL(hash_key->key) : "PHPSESSID");
      return ZEND_HASH_APPLY_KEEP;
    } else {
      sp_log_msg("cookie_encryption", SP_LOG_DROP,
                 "Something went wrong with the decryption of %s.",
                 ZSTR_VAL(hash_key->key) ? ZSTR_VAL(hash_key->key) : "PHPSESSID");
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
zend_string *encrypt_zval(char *data, unsigned long long data_len) {
  const size_t encrypted_msg_len = crypto_secretbox_ZEROBYTES + data_len + 1;
  const size_t emsg_and_nonce_len =pDest
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

  zend_string *z = php_base64_encode(encrypted_data + crypto_secretbox_ZEROBYTES, emsg_and_nonce_len - crypto_secretbox_ZEROBYTES);
  sp_log_err("DEbug","Sess : %s \n", ZSTR_VAL(z));
  return z;
}