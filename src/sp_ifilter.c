#include "php_snuffleupagus.h"

static void (*orig_register_server_variables)(zval *track_vars_array) = NULL;

static const unsigned char sp_hexchars[] = "0123456789ABCDEF";

static const char sp_is_dangerous_char[256] = {
/*     |->  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
/* 0x00 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,
/* 0x10 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x20 */  0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x30 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
/* 0x40 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x50 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x60 */  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x70 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x80 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0x90 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xa0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xb0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xc0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xd0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xe0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 0xf0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void sp_server_strip(HashTable *svars, char *key, int keylen) {
  zval *value = zend_hash_str_find(svars, key, keylen);
  if (!value || Z_TYPE_P(value) != IS_STRING) { return; }

  zend_string *tmp_zstr = Z_STR_P(value);
  char *tmp = ZSTR_VAL(tmp_zstr);
  char *tmpend = tmp + ZSTR_LEN(tmp_zstr);

  for (char *p = tmp; p < tmpend; p++) {
    if (sp_is_dangerous_char[(int)*p]) {
      *p = '_';
    }
  }
}

static void sp_server_encode(HashTable *svars, char *key, int keylen) {
  zval *value = zend_hash_str_find(svars, key, keylen);
  if (!value || Z_TYPE_P(value) != IS_STRING) { return; }
  
  zend_string *tmp_zstr = Z_STR_P(value);
  char *tmp = ZSTR_VAL(tmp_zstr);
  char *tmpend = tmp + ZSTR_LEN(tmp_zstr);
  int extra = 0;
  
  for (char *p = tmp; p < tmpend; p++) {
    extra += sp_is_dangerous_char[(int)*p] * 2;
  }
  if (!extra) { return; }

  zend_string *new_zstr = zend_string_alloc(ZSTR_LEN(tmp_zstr) + extra, 0);
  char *n = ZSTR_VAL(new_zstr);
  for (char *p = tmp; p < tmpend; p++, n++) {
    if (sp_is_dangerous_char[(int)*p]) {
      *n++ = '%';
      *n++ = sp_hexchars[*p >> 4];
      *n = sp_hexchars[*p & 15];
    } else {
      *n = *p;
    }
  }
  ZSTR_VAL(new_zstr)[ZSTR_LEN(new_zstr)] = 0;
  Z_STR_P(value) = new_zstr;

  zend_string_release_ex(tmp_zstr, 0);
}

static void sp_register_server_variables(zval *track_vars_array) {
  orig_register_server_variables(track_vars_array);

  HashTable *svars;
  svars = Z_ARRVAL_P(track_vars_array);


  if (SPCFG(server_encode)) {
    sp_server_encode(svars, ZEND_STRL("REQUEST_URI"));
    sp_server_encode(svars, ZEND_STRL("QUERY_STRING"));
  }

  if (SPCFG(server_strip)) {
    sp_server_strip(svars, ZEND_STRL("PHP_SELF"));
    sp_server_strip(svars, ZEND_STRL("HTTP_HOST"));
    sp_server_strip(svars, ZEND_STRL("HTTP_USER_AGENT"));

    // for cgi + fpm
    sp_server_strip(svars, ZEND_STRL("PATH_INFO"));
    sp_server_strip(svars, ZEND_STRL("PATH_TRANSLATED"));
    sp_server_strip(svars, ZEND_STRL("ORIG_PATH_TRANSLATED"));
    sp_server_strip(svars, ZEND_STRL("ORIG_PATH_INFO"));
  }
}

void sp_hook_register_server_variables()
{
  if (sapi_module.register_server_variables && sapi_module.register_server_variables != sp_register_server_variables) {
    orig_register_server_variables = sapi_module.register_server_variables;
    sapi_module.register_server_variables = sp_register_server_variables;
  }
}
