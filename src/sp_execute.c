#include "php_snuffleupagus.h"

#include <errno.h>
#include <string.h>

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static zend_op_array *(*orig_compile_string)(zval *source_string,
																						 char *filename TSRMLS_DC);
static void (*orig_execute_ex)(zend_execute_data *execute_data);
static int (*orig_zend_stream_open)(const char *filename,
                                    zend_file_handle *handle);

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

static void is_builtin_matching(const char *restrict const filename,
                                char *restrict function_name,
                                char *restrict param_name,
                                sp_list_node *config) {
  if (!config || !config->data) {
    return;
  }

  if (true == should_disable(EG(current_execute_data), function_name, filename,
                             param_name)) {
    sp_terminate();
  }
}

/* This function gets the filename in which `eval()` is called from,
 * since it looks like "foo.php(1) : eval()'d code", so we're starting
 * from the end of the string until the second closing parenthesis. */
char *get_eval_filename(const char *filename) {
  size_t i = strlen(filename);
  int count = 0;
  char *clean_filename = estrdup(filename);

  while (i--) {
    if (clean_filename[i] == '(') {
      if (count == 1) {
        clean_filename[i] = '\0';
        break;
      }
      count++;
    }
  }
  return clean_filename;
}

static void sp_execute_ex(zend_execute_data *execute_data) {
  if (true == should_disable(execute_data, NULL, NULL, NULL)) {
    sp_terminate();
  }

  if (execute_data->func->op_array.type == ZEND_EVAL_CODE) {
    SNUFFLEUPAGUS_G(in_eval) = true;
    sp_list_node *config =
        SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_eval;
    char *filename = get_eval_filename((char *)zend_get_executed_filename());
    is_builtin_matching(filename, "eval", NULL, config);
    efree(filename);
  }

  if (NULL != execute_data->func->op_array.filename) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
      terminate_if_writable(ZSTR_VAL(execute_data->func->op_array.filename));
    }
  }

  orig_execute_ex(execute_data);

  if (true == should_drop_on_ret(execute_data->return_value, execute_data)) {
    sp_terminate();
  }

	SNUFFLEUPAGUS_G(in_eval) = false;
}

static int sp_stream_open(const char *filename, zend_file_handle *handle) {
  zend_execute_data const *const data = EG(current_execute_data);

  if ((NULL == data) || (NULL == data->opline) ||
      (data->func->type != ZEND_USER_FUNCTION)) {
    goto end;
  }

  switch (data->opline->opcode) {
    case ZEND_INCLUDE_OR_EVAL:
      if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
        terminate_if_writable(filename);
      }
      sp_list_node *config =
          SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_include;
      switch (data->opline->extended_value) {
        case ZEND_INCLUDE:
          is_builtin_matching(filename, "include", "inclusion path", config);
          break;
        case ZEND_REQUIRE:
          is_builtin_matching(filename, "require", "inclusion path", config);
          break;
        case ZEND_REQUIRE_ONCE:
          is_builtin_matching(filename, "require_once", "inclusion path",
                              config);
          break;
        case ZEND_INCLUDE_ONCE:
          is_builtin_matching(filename, "include_once", "inclusion path",
                              config);
          break;
        case ZEND_EVAL:
          is_builtin_matching(filename, "eval", NULL, config);
          break;
        default:
          break;
      }
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

  /* zend_execute_internal is used for "indirect" functions call,
   * like array_map or call_user_func. */

  return SUCCESS;
}
