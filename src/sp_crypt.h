#ifndef __SP_CRYPT
#define __SP_CRYPT

#include "SAPI.h"
#include "tweetnacl.h"

#include "sp_utils.h"

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/standard/base64.h"

void generate_key(unsigned char *key);
int decrypt_zval(zval *pDest, bool simulation, zend_hash_key *hask_key);
zend_string *encrypt_zval(char *data, unsigned long long data_len);

#endif /*__SP_CRYPT */