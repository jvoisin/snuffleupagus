#if PHP_VERSION_ID < 80000

// copied from PHP 8.0.9 sources
ZEND_API zend_string *zend_string_concat2(
	const char *str1, size_t str1_len,
	const char *str2, size_t str2_len);

#define ZEND_HASH_REVERSE_FOREACH_KEY_PTR(ht, _h, _key, _ptr) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key; \
	_ptr = Z_PTR_P(_z);

#endif

#if PHP_VERSION_ID < 70300

// copied from PHP 7.4.22 sources

static zend_always_inline uint32_t zend_gc_delref(zend_refcounted_h *p) {
	ZEND_ASSERT(p->refcount > 0);
	// ZEND_RC_MOD_CHECK(p);
	return --(p->refcount);
}
#define GC_DELREF(p)				zend_gc_delref(&(p)->gc)

static zend_always_inline void zend_string_release_ex(zend_string *s, int persistent)
{
	if (!ZSTR_IS_INTERNED(s)) {
		if (GC_DELREF(s) == 0) {
			if (persistent) {
				ZEND_ASSERT(GC_FLAGS(s) & IS_STR_PERSISTENT);
				free(s);
			} else {
				ZEND_ASSERT(!(GC_FLAGS(s) & IS_STR_PERSISTENT));
				efree(s);
			}
		}
	}
}

static zend_always_inline void zend_string_efree(zend_string *s)
{
	ZEND_ASSERT(!ZSTR_IS_INTERNED(s));
	ZEND_ASSERT(GC_REFCOUNT(s) <= 1);
	ZEND_ASSERT(!(GC_FLAGS(s) & IS_STR_PERSISTENT));
	efree(s);
}

#endif

#if PHP_VERSION_ID < 70200

#undef ZEND_HASH_REVERSE_FOREACH

// copied from PHP 7.4.22 sources

#define ZEND_HASH_REVERSE_FOREACH(_ht, indirect) do { \
	HashTable *__ht = (_ht); \
	uint32_t _idx = __ht->nNumUsed; \
	Bucket *_p = __ht->arData + _idx; \
	zval *_z; \
	for (_idx = __ht->nNumUsed; _idx > 0; _idx--) { \
		_p--; \
		_z = &_p->val; \
		if (indirect && Z_TYPE_P(_z) == IS_INDIRECT) { \
			_z = Z_INDIRECT_P(_z); \
		} \
		if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;


#define ZEND_HASH_FOREACH_END_DEL() \
			__ht->nNumOfElements--; \
			do { \
				uint32_t j = HT_IDX_TO_HASH(_idx - 1); \
				uint32_t nIndex = _p->h | __ht->nTableMask; \
				uint32_t i = HT_HASH(__ht, nIndex); \
				if (UNEXPECTED(j != i)) { \
					Bucket *prev = HT_HASH_TO_BUCKET(__ht, i); \
					while (Z_NEXT(prev->val) != j) { \
						i = Z_NEXT(prev->val); \
						prev = HT_HASH_TO_BUCKET(__ht, i); \
					} \
					Z_NEXT(prev->val) = Z_NEXT(_p->val); \
				} else { \
					HT_HASH(__ht, nIndex) = Z_NEXT(_p->val); \
				} \
			} while (0); \
		} \
		__ht->nNumUsed = _idx; \
	} while (0)

#endif