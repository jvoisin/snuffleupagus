#include "php_snuffleupagus.h"

PHP_FUNCTION(sp_libxml_disable_entity_loader) { RETURN_TRUE; }

int hook_libxml_disable_entity_loader() {
  zval func_name;
  zval hmac;
  zval params[1];

  TSRMLS_FETCH();

  /* Call the php function here instead of re-implementing it is a bit
   * ugly, but we do not want to introduce compile-time dependencies against
   * libxml. */
  ZVAL_STRING(&func_name, "libxml_disable_entity_loader");
  ZVAL_STRING(&params[0], "true");
  call_user_function(CG(function_table), NULL, &func_name, &hmac, 1, params);

  HOOK_FUNCTION("libxml_disable_entity_loader", sp_internal_functions_hook,
                PHP_FN(sp_libxml_disable_entity_loader));

  return SUCCESS;
}
