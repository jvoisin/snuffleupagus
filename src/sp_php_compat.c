#include "php_snuffleupagus.h"

#if PHP_VERSION_ID < 80000

// zend_string_concat2 taken from PHP 8.0.9 zend_string.c
// TODO: license clarification

ZEND_API zend_string *zend_string_concat2(
		const char *str1, size_t str1_len,
		const char *str2, size_t str2_len)
{
	size_t len = str1_len + str2_len;
	zend_string *res = zend_string_alloc(len, 0);

	memcpy(ZSTR_VAL(res), str1, str1_len);
	memcpy(ZSTR_VAL(res) + str1_len, str2, str2_len);
	ZSTR_VAL(res)[len] = '\0';

	return res;
}

#endif
