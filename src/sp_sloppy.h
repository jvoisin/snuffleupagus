#ifndef SP_SLOPPY_H
#define SP_SLOPPY_H
#include "php_snuffleupagus.h"
#include "zend_vm.h"

void hook_sloppy(void);
void sp_sloppy_modify_opcode(zend_op_array* opline);

#endif
