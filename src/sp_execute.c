#include "php_snuffleupagus.h"

#include <errno.h>
#include <string.h>

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus);

static void (*orig_execute_ex)(zend_execute_data *execute_data);
static int (*orig_zend_stream_open)(const char *filename,
                                             zend_file_handle *handle);

// FIXME handle symlink
ZEND_COLD static inline void terminate_if_writable(const char *filename) {
  if (0 == access(filename, W_OK)) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->simulation) {
      sp_log_msg("readonly_exec", SP_LOG_NOTICE,
        "Attempted execution of a writable file (%s).", filename);
    } else {
      sp_log_msg("readonly_exec", SP_LOG_DROP,
        "Attempted execution of a writable file (%s).", filename);
      sp_terminate();
    }
  } else {
    if (EACCES != errno) {
      sp_log_err("Writable execution", "Error while accessing %s: %s", filename,
        strerror(errno));
    }
  }
}

static void check_inclusion_regexp(const char * const filename) {
  if (SNUFFLEUPAGUS_G(config).config_regexp_inclusion->regexp_inclusion) {
    const sp_node_t* config = SNUFFLEUPAGUS_G(config).config_regexp_inclusion->regexp_inclusion;
    if (!config || !config->data) {
      return;
    }
    while (config) {
      pcre *config_node = (pcre*)(config->data);
      if (false == is_regexp_matching(config_node, filename)) {
        sp_log_msg("include", SP_LOG_DROP, "Inclusion of a forbidden file (%s).", filename);
        sp_terminate();
      }
      config = config->next;
    }
  }
}

static void sp_execute_ex(zend_execute_data *execute_data) {
  if (NULL == execute_data->func->common.function_name) {
    goto execute;
  }

  if (true == should_disable(execute_data)) {
    return;
  }

  if (execute_data->func->op_array.type == ZEND_EVAL_CODE) {
    sp_log_debug("Currently in an eval\n");
  }

  if (NULL != execute_data->func->op_array.filename) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
      terminate_if_writable(ZSTR_VAL(execute_data->func->op_array.filename));
    }
}

execute:
  orig_execute_ex(execute_data);
}

static int sp_stream_open(const char *filename,
                                   zend_file_handle *handle) {
  const zend_execute_data *data = EG(current_execute_data);

  if ((NULL != data) && (NULL != data->opline) && 
      (ZEND_INCLUDE_OR_EVAL == data->opline->opcode)) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
      terminate_if_writable(filename);
    }
    check_inclusion_regexp(filename);
  }
  return orig_zend_stream_open(filename, handle);
}

int hook_execute(void) {
  TSRMLS_FETCH();

  /* zend_execute_ex is used for "classic" function calls */
  orig_execute_ex = zend_execute_ex;
  zend_execute_ex = sp_execute_ex;

  /* zend_stream_open_function is used FIXME */
  orig_zend_stream_open = zend_stream_open_function;
  zend_stream_open_function = sp_stream_open;

  /* zend_execute_internal is used for "indirect" functions call,
   * like array_map or call_user_func. */

  return SUCCESS;
}
