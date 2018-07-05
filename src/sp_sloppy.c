#include "sp_sloppy.h"

ZEND_API void (*default_zend_execute_ex)(zend_execute_data*) = NULL;

ZEND_API void zend_execute_hook(zend_execute_data* ex) {
  zend_op* orig_opline = (NULL != ex) ? (void*)ex->opline : NULL;

  if (NULL != orig_opline) {
    for (; NULL != orig_opline->handler; orig_opline++) {
      if (orig_opline->opcode == ZEND_IS_EQUAL) {
        orig_opline->opcode = ZEND_IS_IDENTICAL;
        zend_vm_set_opcode_handler(orig_opline);
      } else if (orig_opline->opcode == ZEND_IS_NOT_EQUAL) {
        orig_opline->opcode = ZEND_IS_NOT_IDENTICAL;
        zend_vm_set_opcode_handler(orig_opline);
      }
    }
  }

  if (default_zend_execute_ex)
    default_zend_execute_ex(ex);
}
