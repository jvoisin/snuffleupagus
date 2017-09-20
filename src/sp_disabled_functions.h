#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/standard/md5.h"

int hook_disabled_functions();
bool should_disable(zend_execute_data* function_name);

#endif /* __SP_DISABLE_FUNCTIONS_H */
