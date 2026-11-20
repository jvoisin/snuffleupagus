#include "php_snuffleupagus.h"

void sp_sloppy_modify_opcode(zend_op_array* opline) {
  if (NULL == opline) {
    return;
  }
  for (size_t i = 0; i < opline->last; i++) {
    zend_op* orig_opline = &(opline->opcodes[i]);
    if (orig_opline->opcode == ZEND_IS_EQUAL) {
      orig_opline->opcode = ZEND_IS_IDENTICAL;
      zend_vm_set_opcode_handler(orig_opline);
    } else if (orig_opline->opcode == ZEND_IS_NOT_EQUAL) {
      orig_opline->opcode = ZEND_IS_NOT_IDENTICAL;
      zend_vm_set_opcode_handler(orig_opline);
    }
  }
}

static void array_handler(INTERNAL_FUNCTION_PARAMETERS, const char* name,
                          size_t size, const char* spec) {
  zif_handler handler;
  zval params[3] = {0};
  zval *value, *array = NULL;
  zend_bool strict = 0;
  uint32_t nb_params = ZEND_NUM_ARGS();

  zend_parse_parameters(nb_params, spec, &value, &array, &strict);

  ZVAL_COPY(&params[0], value);
  ZVAL_BOOL(&params[2], 1);  // we want to always have strict mode enabled

  if (array) {
    ZVAL_COPY(&params[1], array);
    // Lie about the number of parameters,
    // since we are always passing strict = 1
    nb_params = 3;
  } else {
    ZVAL_NULL(&params[1]);
  }

  handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook),
                                   name, size);
#ifdef ZTS
  zif_handler hooked_handler = zend_hash_str_find_ptr(
      SPG(internal_function_handlers), name, size);
  zend_hash_str_del(SPG(internal_function_handlers), name, size);
  zend_function *func = zend_hash_str_find_ptr(CG(function_table), name, size);
  zend_function local_func = *func;
  local_func.internal_function.handler = handler;

  zend_fcall_info fci;
  zend_fcall_info_cache fcc;
  memset(&fci, 0, sizeof(fci));
  memset(&fcc, 0, sizeof(fcc));
  fci.size = sizeof(fci);
  fci.retval = return_value;
  fci.param_count = nb_params;
  fci.params = params;
#if PHP_VERSION_ID < 70300
  fcc.initialized = 1;
#endif
  fcc.function_handler = &local_func;
  zend_call_function(&fci, &fcc);
  if (hooked_handler) {
    zend_hash_str_update_ptr(SPG(internal_function_handlers), name, size,
                             hooked_handler);
  }
#else
  zval func_name;
  zend_function *func = zend_hash_str_find_ptr(CG(function_table), name, size);
  zif_handler current_handler = func->internal_function.handler;
  func->internal_function.handler = handler;
  ZVAL_STRING(&func_name, name);
  call_user_function(CG(function_table), NULL, &func_name, return_value,
                     nb_params, params);
  func->internal_function.handler = current_handler;
  zval_ptr_dtor(&func_name);
#endif

  zval_ptr_dtor(&params[0]);
  zval_ptr_dtor(&params[1]);
  zval_ptr_dtor(&params[2]);
}

PHP_FUNCTION(sp_in_array) {
  array_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, "in_array",
                sizeof("in_array") - 1, "zz|b");
}
PHP_FUNCTION(sp_array_search) {
  array_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, "array_search",
                sizeof("array_search") - 1, "zz|b");
}
PHP_FUNCTION(sp_array_keys) {
  array_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, "array_keys",
                sizeof("array_keys") - 1, "z|zb");
}

void hook_sloppy() {
  TSRMLS_FETCH();

  HOOK_FUNCTION("in_array", sp_internal_functions_hook, PHP_FN(sp_in_array));
  HOOK_FUNCTION("array_search", sp_internal_functions_hook,
                PHP_FN(sp_array_search));
  HOOK_FUNCTION("array_keys", sp_internal_functions_hook,
                PHP_FN(sp_array_keys));
}
