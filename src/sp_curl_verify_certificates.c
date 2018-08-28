#include "php_snuffleupagus.h"
#include "sp_config.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

PHP_FUNCTION(sp_curl_setopt) {
	void (*orig_handler)(INTERNAL_FUNCTION_PARAMETERS);
	zend_string *protocol_name = NULL;
	zval *zid, *zvalue;
	zend_long options;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_RESOURCE(zid)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	__asm__("int3");

	if (option & CURLOPT_SSL_VERIFYCLIENT) {
		if (zval_get_long(zvalue) != 1) {
			sp_log_err("verify_vertificates", "Please don't deactivate server certificate validation");
		}
	} else if (option & CURLOPT_SSL_VERIFYHOST) {
		if (zval_get_long(zvalue) != 2) {
			sp_log_err("verify_vertificates", "Please don't deactivate client certificate validation");
		}
	}

	orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

int hook_curl_verify_certificates() {
	TSRMLS_FETCH();

  HOOK_FUNCTION("curl_setopt", sp_internal_functions_hook, PHP_FN(sp_curl_setopt));

	return SUCCESS;
}
