#include "php_snuffleupagus.h"

PHP_FUNCTION(sp_libxml_disable_entity_loader) {
  sp_log_warn("xxe", "A call to libxml_disable_entity_loader was tried and nopped");
  RETURN_TRUE;
}

PHP_FUNCTION(sp_libxml_set_external_entity_loader) {
  sp_log_warn("xxe", "A call to libxml_set_external_entity_loader was tried and nopped");
  RETURN_TRUE;
}

int hook_libxml_disable_entity_loader() {
  TSRMLS_FETCH();

  if (!zend_hash_str_find_ptr(&module_registry, ZEND_STRL("xml"))) {
    sp_log_warn("xxe", "Cannot enable XXE protection. XML support is disabled in PHP.");
  }

  zval func_name;
  zval retval;
  zval params[1] = {0};

#if PHP_VERSION_ID < 80000
  // This function is deprecated in PHP8, but better safe than sorry for php7.
  ZVAL_STRING(&func_name, "libxml_disable_entity_loader");
  ZVAL_STRING(&params[0], "true");
  call_user_function(CG(function_table), NULL, &func_name, &retval, 1, params);
#endif

  // This is now the recommended way to disable external entities
  ZVAL_STRING(&func_name, "libxml_set_external_entity_loader");
  ZVAL_NULL(&params[0]);
  call_user_function(CG(function_table), NULL, &func_name, &retval, 1, params);

  HOOK_FUNCTION("libxml_disable_entity_loader", sp_internal_functions_hook, PHP_FN(sp_libxml_disable_entity_loader));
  HOOK_FUNCTION("libxml_set_external_entity_loader", sp_internal_functions_hook, PHP_FN(sp_libxml_set_external_entity_loader));

  return SUCCESS;
}
