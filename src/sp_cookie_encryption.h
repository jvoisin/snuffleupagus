
#ifndef __SP_COOKIE_ENCRYPTION
#define __SP_COOKIE_ENCRYPTION

#include "SAPI.h"
#include "tweetnacl.h"

#include "sp_utils.h"

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/standard/base64.h"

#define SAMESITE_COOKIE_FORMAT "; samesite="

int hook_cookies();
int decrypt_cookie(zval *pDest, int num_args, va_list args,
                   zend_hash_key *hash_key);

#endif /* __SP_COOKIE_ENCRYPTION */
