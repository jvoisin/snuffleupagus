#include "php_snuffleupagus.h"

#include <errno.h>
#include <string.h>

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static void (*orig_execute_ex)(zend_execute_data *execute_data);
static int (*orig_zend_stream_open)(const char *filename,
                                             zend_file_handle *handle);
static int (*orig_zend_write)(const char *str, size_t len);

// FIXME handle symlink
ZEND_COLD static inline void terminate_if_writable(const char *filename) {
  if (0 == access(filename, W_OK)) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->simulation) {
      sp_log_msg("readonly_exec", SP_LOG_SIMULATION,
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

static void construct_include_handler(const char * const filename) {
  if (SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_include) {
    const sp_node_t* config = SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_include;
    if (!config || !config->data) {
      return;
    }

    while (config) {
      sp_disabled_function *config_node = (sp_disabled_function*)(config->data);
      if (true == sp_match_value(filename, config_node->value, config_node->value_r)) {
        if (true == config_node->allow) {
          return;
        }
        sp_log_disable("include", "inclusion path", filename, config_node);
        if (false == config_node->simulation) {
          sp_terminate();
        }
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
    sp_terminate();
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

static int hook_zend_write(const char *str, size_t len) {
  const sp_node_t* config =
    SNUFFLEUPAGUS_G(config).config_disabled_functions->disabled_functions;

  if (!str) {
    goto end;
  }

  while (config) {
    sp_disabled_function *config_node = (sp_disabled_function*)(config->data);
    if (config_node && config_node->function) {
      if (0 == strcmp("echo", config_node->function)) {
        if (config_node->allow) {
          break;
        } else if ((!config_node->value && !config_node->value_r) ||
                   (true == sp_match_value(str, config_node->value, config_node->value_r))) {
          sp_log_disable("echo", NULL, str, config_node);
          if (false == config_node->simulation) {
            sp_terminate();
          }
        }
      }
    }
    config = config->next;
  }

end:
  return orig_zend_write(str, len);
}

static int sp_stream_open(const char *filename, zend_file_handle *handle) {
  zend_execute_data const * const data = EG(current_execute_data);

  if ((NULL == data) || (NULL == data->opline) || (data->func->type != ZEND_USER_FUNCTION)) {
    goto end;
  }

  switch(data->opline->opcode) {
    case ZEND_INCLUDE_OR_EVAL:
      if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
        terminate_if_writable(filename);
      }
      construct_include_handler(filename);
  }

end:
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

  /* zend_write is used for "echo" and "print",
   * so it is used every time php output something (almost). */
  orig_zend_write = zend_write;
  zend_write = hook_zend_write;

  /* zend_execute_internal is used for "indirect" functions call,
   * like array_map or call_user_func. */

  return SUCCESS;
}
