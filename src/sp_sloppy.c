#include "sp_sloppy.h"

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

void hook_sloppy() {
  if (NULL == orig_zend_compile_file) {
    orig_zend_compile_file = zend_compile_file;
    zend_compile_file = sp_compile_file;
  }

  if (NULL == orig_zend_compile_string) {
    orig_zend_compile_string = zend_compile_string;
    zend_compile_string = sp_compile_string;
  }
}
