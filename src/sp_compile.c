#include "php_snuffleupagus.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus);

static zend_op_array *(*orig_compile_file)(zend_file_handle *, int);
static zend_op_array *(*orig_compile_string)(zval *, char *);

zend_op_array *sp_compile_file(zend_file_handle *file_handle, int type) {
  zend_op_array *ret = orig_compile_file(file_handle, type);
  
  const sp_node_t* config = SNUFFLEUPAGUS_G(config).config_disabled_functions->disabled_functions;

  while (config && config->data) {
    const char* function_name = ((sp_disabled_function*)config->data)->function;
    const pcre* function_name_regexp = ((sp_disabled_function*)config->data)->r_function;

    sp_log_err("checking for %s", function_name);
    //  EG(function_table)->arData[count - 1].val.value.func->internal_function.handler = PHP_FN(check_disabled_function);
    
    if (function_name) {
      if (HOOK_FUNCTION(function_name, disabled_functions_hook, PHP_FN(check_disabled_function), true) == SUCCESS) {
          sp_log_err("Successfully hooked %s", function_name);
      }
      break;
    } else if (function_name_regexp) {
      sp_log_err("error", "We'll hook regard later.");
    }
    config = config->next;
  }
  
  return ret;
}

zend_op_array *sp_compile_string(zval *source_string, char *filename) {
  zend_op_array *ret = orig_compile_string(source_string, filename);
  sp_log_err("in compile_string : filename is :%s", filename);
  return ret;
}


int hook_compile(void) {
  TSRMLS_FETCH();

  /* zend_compile_file is used to compile php file */
  orig_compile_file = zend_compile_file;
  zend_compile_file = sp_compile_file;

  /* zend_compile_string is used to compile php string */
  orig_compile_string = zend_compile_string;
  zend_compile_string = sp_compile_string;

  return SUCCESS;
}