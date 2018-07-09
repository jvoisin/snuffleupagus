#include "php_snuffleupagus.h"
#include "zend_vm.h"
#include <errno.h>
#include <string.h>

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static void (*orig_execute_ex)(zend_execute_data *execute_data);
static void (*orig_zend_execute_internal)(zend_execute_data *execute_data,
                                          zval *return_value);
static int (*orig_zend_stream_open)(const char *filename,
                                    zend_file_handle *handle);

// FIXME handle symlink
ZEND_COLD static inline void terminate_if_writable(const char *filename) {
  if (0 == access(filename, W_OK)) {
    if (SNUFFLEUPAGUS_G(config).config_readonly_exec->dump) {
      sp_log_request(
          SNUFFLEUPAGUS_G(config).config_readonly_exec->dump,
          SNUFFLEUPAGUS_G(config).config_readonly_exec->textual_representation,
          SP_TOKEN_READONLY_EXEC);
    }
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

inline static void is_builtin_matching(const char *restrict const filename,
                                       const char *restrict const function_name,
                                       const char *restrict const param_name,
                                       const sp_list_node *config) {
  if (!config || !config->data) {
    return;
  }

  if (true == should_disable_ht(EG(current_execute_data), function_name, filename,
                             param_name, SNUFFLEUPAGUS_G(config)
                             .config_disabled_functions_reg->disabled_functions,
                             SNUFFLEUPAGUS_G(config).config_disabled_functions)) {
    sp_terminate();
  }
}

static void ZEND_HOT
is_in_eval_and_whitelisted(const zend_execute_data *execute_data) {
  sp_config_eval *eval = SNUFFLEUPAGUS_G(config).config_eval;

  if (EXPECTED(0 == SNUFFLEUPAGUS_G(in_eval))) {
    return;
  }

  if (EXPECTED(NULL == SNUFFLEUPAGUS_G(config).config_eval->whitelist)) {
    return;
  }

  if (zend_is_executing() && !EG(current_execute_data)->func) {
    return;
  }

  if (UNEXPECTED(!(execute_data->func->common.function_name))) {
    return;
  }

  char const *const current_function = ZSTR_VAL(EX(func)->common.function_name);

  if (EXPECTED(NULL != current_function)) {
    if (UNEXPECTED(false == check_is_in_eval_whitelist(current_function))) {
      if (eval->dump) {
        sp_log_request(
            SNUFFLEUPAGUS_G(config).config_eval->dump,
            SNUFFLEUPAGUS_G(config).config_eval->textual_representation,
            SP_TOKEN_EVAL_WHITELIST);
      }
      if (eval->simulation) {
        sp_log_msg(
            "Eval_whitelist", SP_LOG_SIMULATION,
            "The function '%s' isn't in the eval whitelist, logging its call.",
            current_function);
        return;
      } else {
        sp_log_msg(
            "Eval_whitelist", SP_LOG_DROP,
            "The function '%s' isn't in the eval whitelist, dropping its call.",
            current_function);
        sp_terminate();
      }
    }
  }
}

/* This function gets the filename in which `eval()` is called from,
 * since it looks like "foo.php(1) : eval()'d code", so we're starting
 * from the end of the string until the second closing parenthesis. */
char *get_eval_filename(const char *const filename) {
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
  is_in_eval_and_whitelisted(execute_data);

  if (!execute_data) {
    return;
  }

  if (UNEXPECTED(EX(func)->op_array.type == ZEND_EVAL_CODE)) {
    const sp_list_node *config = zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(config)
        .config_disabled_functions, "eval", 4);
    char *filename = get_eval_filename((char *)zend_get_executed_filename());
    is_builtin_matching(filename, "eval", NULL, config);
    efree(filename);

    SNUFFLEUPAGUS_G(in_eval)++;
    orig_execute_ex(execute_data);
    SNUFFLEUPAGUS_G(in_eval)--;
    return;
  }

  if (!execute_data->prev_execute_data ||
      !execute_data->prev_execute_data->func ||
      !ZEND_USER_CODE(execute_data->prev_execute_data->func->type) ||
      !execute_data->prev_execute_data->opline) {
    if (UNEXPECTED(true == should_disable_ht(execute_data, NULL, NULL, NULL,
            SNUFFLEUPAGUS_G(config).config_disabled_functions_reg->disabled_functions,
            SNUFFLEUPAGUS_G(config).config_disabled_functions))) {
      sp_terminate();
    }
  } else if ((execute_data->prev_execute_data->opline->opcode ==
                  ZEND_DO_FCALL ||
              execute_data->prev_execute_data->opline->opcode ==
                  ZEND_DO_UCALL ||
              execute_data->prev_execute_data->opline->opcode ==
                  ZEND_DO_FCALL_BY_NAME)) {
    if (UNEXPECTED(true == should_disable_ht(execute_data, NULL, NULL, NULL,
            SNUFFLEUPAGUS_G(config).config_disabled_functions_reg->disabled_functions,
            SNUFFLEUPAGUS_G(config).config_disabled_functions))) {
      sp_terminate();
    }
  }

  if (NULL != EX(func)->op_array.filename) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
      terminate_if_writable(ZSTR_VAL(EX(func)->op_array.filename));
    }
  }

  orig_execute_ex(execute_data);

  if (UNEXPECTED(true == should_drop_on_ret_ht(EX(return_value), execute_data,
          SNUFFLEUPAGUS_G(config).config_disabled_functions_reg_ret
          ->disabled_functions,
          SNUFFLEUPAGUS_G(config).config_disabled_functions_ret))) {
    sp_terminate();
  }
}

static void sp_zend_execute_internal(INTERNAL_FUNCTION_PARAMETERS) {
  is_in_eval_and_whitelisted(execute_data);

  if (UNEXPECTED(NULL != orig_zend_execute_internal)) {
    orig_zend_execute_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  } else {
    EX(func)->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  }
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
      switch (data->opline->extended_value) {
        case ZEND_INCLUDE:
          is_builtin_matching(filename, "include", "inclusion path",
              zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(config)
                .config_disabled_functions, "include", 7));
          break;
        case ZEND_REQUIRE:
          is_builtin_matching(filename, "require", "inclusion path",
              zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(config)
                .config_disabled_functions, "require", 7));
          break;
        case ZEND_REQUIRE_ONCE:
          is_builtin_matching(filename, "require_once", "inclusion path",
              zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(config)
                .config_disabled_functions, "require_once", 12));
          break;
        case ZEND_INCLUDE_ONCE:
          is_builtin_matching(filename, "include_once", "inclusion path",
              zend_hash_str_find_ptr(SNUFFLEUPAGUS_G(config)
                .config_disabled_functions, "include_once", 12));
          break;
          EMPTY_SWITCH_DEFAULT_CASE();
      }
  }

end:
  return orig_zend_stream_open(filename, handle);
}

int hook_execute(void) {
  TSRMLS_FETCH();

  /* zend_execute_ex is used for "user" function calls */
  orig_execute_ex = zend_execute_ex;
  zend_execute_ex = sp_execute_ex;

  /* zend_execute_internal is used for "builtin" functions calls */
  orig_zend_execute_internal = zend_execute_internal;
  zend_execute_internal = sp_zend_execute_internal;

  /* zend_stream_open_function is used for include-related stuff */
  orig_zend_stream_open = zend_stream_open_function;
  zend_stream_open_function = sp_stream_open;

  return SUCCESS;
}
