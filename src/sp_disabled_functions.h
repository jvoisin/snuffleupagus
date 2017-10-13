#ifndef __SP_DISABLE_FUNCTIONS_H
#define __SP_DISABLE_FUNCTIONS_H

int hook_disabled_functions();
bool should_disable(zend_execute_data* function_name);

#endif /* __SP_DISABLE_FUNCTIONS_H */
