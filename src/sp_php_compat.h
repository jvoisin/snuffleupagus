#if PHP_VERSION_ID < 80000
ZEND_API zend_string *zend_string_concat2(
    const char *str1, size_t str1_len,
    const char *str2, size_t str2_len);

#define ZEND_HASH_REVERSE_FOREACH_KEY_PTR(ht, _h, _key, _ptr) \
    ZEND_HASH_REVERSE_FOREACH(ht, 0); \
    _h = _p->h; \
    _key = _p->key; \
    _ptr = Z_PTR_P(_z);

#endif