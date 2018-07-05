#ifndef SP_SLOPPY_H
#include "SAPI.h"
#include "zend_vm.h"

extern ZEND_API void (*default_zend_execute_ex)(zend_execute_data*);
ZEND_API void zend_execute_hook(zend_execute_data* ex);

#endif /* SP_SLOPPY_H */
