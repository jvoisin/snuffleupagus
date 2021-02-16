#include "SAPI.h"
#include "tweetnacl.h"

#include "sp_utils.h"

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/standard/base64.h"

void hook_session(void);
