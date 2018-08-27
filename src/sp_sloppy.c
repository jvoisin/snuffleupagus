#include "php_snuffleupagus.h"
#include "sp_sloppy.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

ZEND_API zend_op_array* (*orig_zend_compile_file)(zend_file_handle* file_handle,
                                                  int type) = NULL;
ZEND_API zend_op_array* (*orig_zend_compile_string)(zval* source_string,
                                                    char* filename) = NULL;

static void modify_opcode(zend_op_array* opline) {
  if (NULL != opline) {
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
}

ZEND_API zend_op_array* sp_compile_string(zval* source_string, char* filename) {
  zend_op_array* opline = orig_zend_compile_string(source_string, filename);
  modify_opcode(opline);
  return opline;
}

ZEND_API zend_op_array* sp_compile_file(zend_file_handle* file_handle,
                                        int type) {
  zend_op_array* opline = orig_zend_compile_file(file_handle, type);
  modify_opcode(opline);
  return opline;
}

PHP_FUNCTION(sp_in_array) {
  void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
  zval func_name;
  zval params[3] = {{{0}}};
  zend_bool dummy;

  zend_parse_parameters(ZEND_NUM_ARGS(), "zz|b", &params[0], &params[1], &dummy);

  ZVAL_BOOL(&params[2], 1);
  ZVAL_STRING(&func_name, "in_array");

  handler = zend_hash_str_find_ptr(
      SNUFFLEUPAGUS_G(sp_internal_functions_hook), "in_array", sizeof("in_array") - 1);
  zend_internal_function *func = zend_hash_str_find_ptr(
      CG(function_table), "in_array", sizeof("in_array") - 1);
  func->handler = handler;

  call_user_function(CG(function_table), NULL, &func_name, return_value, 3,
                     params);

  func->handler = PHP_FN(sp_in_array);
}

void hook_sloppy() {
  TSRMLS_FETCH();

  if (NULL == orig_zend_compile_file) {
    orig_zend_compile_file = zend_compile_file;
    zend_compile_file = sp_compile_file;
  }

  if (NULL == orig_zend_compile_string) {
    orig_zend_compile_string = zend_compile_string;
    zend_compile_string = sp_compile_string;
  }

  HOOK_FUNCTION("in_array", sp_internal_functions_hook, PHP_FN(sp_in_array));
}
