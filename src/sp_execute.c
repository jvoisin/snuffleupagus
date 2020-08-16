#include "php_snuffleupagus.h"

static void (*orig_execute_ex)(zend_execute_data *execute_data) = NULL;
static void (*orig_zend_execute_internal)(zend_execute_data *execute_data,
                                          zval *return_value) = NULL;
static int (*orig_zend_stream_open)(const char *filename,
                                    zend_file_handle *handle) = NULL;

// FIXME handle symlink
ZEND_COLD static inline void terminate_if_writable(const char *filename) {
  const sp_config_readonly_exec *config_ro_exec =
      SNUFFLEUPAGUS_G(config).config_readonly_exec;

  if (0 == access(filename, W_OK)) {
    if (config_ro_exec->dump) {
      sp_log_request(config_ro_exec->dump,
                     config_ro_exec->textual_representation,
                     SP_TOKEN_READONLY_EXEC);
    }
    if (true == config_ro_exec->simulation) {
      sp_log_simulation("readonly_exec",
                        "Attempted execution of a writable file (%s).",
                        filename);
    } else {
      sp_log_drop("readonly_exec",
                  "Attempted execution of a writable file (%s).", filename);
    }
  } else {
    if (EACCES != errno) {
      // LCOV_EXCL_START
      sp_log_err("Writable execution", "Error while accessing %s: %s", filename,
                 strerror(errno));
      // LCOV_EXCL_STOP
    }
  }
}

inline static void is_builtin_matching(
    const zend_string *restrict const param_value,
    const char *restrict const function_name,
    const char *restrict const param_name, const sp_list_node *config,
    const HashTable *ht) {
  if (!config || !config->data) {
    return;
  }

  should_disable_ht(
      EG(current_execute_data), function_name, param_value, param_name,
      SNUFFLEUPAGUS_G(config).config_disabled_functions_reg->disabled_functions,
      ht);
}

static void ZEND_HOT
is_in_eval_and_whitelisted(const zend_execute_data *execute_data) {
  const sp_config_eval *config_eval = SNUFFLEUPAGUS_G(config).config_eval;

  if (EXPECTED(0 == SNUFFLEUPAGUS_G(in_eval))) {
    return;
  }

  if (EXPECTED(NULL == SNUFFLEUPAGUS_G(config).config_eval->whitelist)) {
    return;
  }

  if (zend_is_executing() && !EG(current_execute_data)->func) {
    return;  // LCOV_EXCL_LINE
  }

  if (UNEXPECTED(!(execute_data->func->common.function_name))) {
    return;
  }

  zend_string const *const current_function = EX(func)->common.function_name;

  if (EXPECTED(NULL != current_function)) {
    if (UNEXPECTED(false == check_is_in_eval_whitelist(current_function))) {
      if (config_eval->dump) {
        sp_log_request(config_eval->dump, config_eval->textual_representation,
                       SP_TOKEN_EVAL_WHITELIST);
      }
      if (config_eval->simulation) {
        sp_log_simulation(
            "Eval_whitelist",
            "The function '%s' isn't in the eval whitelist, logging its call.",
            ZSTR_VAL(current_function));
        return;
      } else {
        sp_log_drop(
            "Eval_whitelist",
            "The function '%s' isn't in the eval whitelist, dropping its call.",
            ZSTR_VAL(current_function));
      }
    }
  }
}

/* This function gets the filename in which `eval()` is called from,
 * since it looks like "foo.php(1) : eval()'d code", so we're starting
 * from the end of the string until the second closing parenthesis. */
zend_string *get_eval_filename(const char *const filename) {
  int count = 0;
  zend_string *clean_filename = zend_string_init(filename, strlen(filename), 0);

  for (int i = ZSTR_LEN(clean_filename); i >= 0; i--) {
    if (ZSTR_VAL(clean_filename)[i] == '(') {
      if (count == 1) {
        ZSTR_VAL(clean_filename)[i] = '\0';
        clean_filename = zend_string_truncate(clean_filename, i, 0);
        break;
      }
      count++;
    }
  }
  return clean_filename;
}

static void sp_execute_ex(zend_execute_data *execute_data) {
  is_in_eval_and_whitelisted(execute_data);
  const HashTable *config_disabled_functions =
      SNUFFLEUPAGUS_G(config).config_disabled_functions;

  if (!execute_data) {
    return;  // LCOV_EXCL_LINE
  }

  if (UNEXPECTED(EX(func)->op_array.type == ZEND_EVAL_CODE)) {
    const sp_list_node *config = zend_hash_str_find_ptr(
        config_disabled_functions, "eval", sizeof("eval") - 1);

    zend_string *filename = get_eval_filename(zend_get_executed_filename());
    is_builtin_matching(filename, "eval", NULL, config,
                        config_disabled_functions);
    zend_string_release(filename);

    SNUFFLEUPAGUS_G(in_eval)++;
    orig_execute_ex(execute_data);
    SNUFFLEUPAGUS_G(in_eval)--;
    return;
  }

  if (NULL != EX(func)->op_array.filename) {
    if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
      terminate_if_writable(ZSTR_VAL(EX(func)->op_array.filename));
    }
  }

  if (SNUFFLEUPAGUS_G(config).hook_execute) {
    char *function_name = get_complete_function_path(execute_data);
    zval ret_val;
    const sp_list_node *config_disabled_functions_reg =
        SNUFFLEUPAGUS_G(config)
            .config_disabled_functions_reg->disabled_functions;

    if (!function_name) {
      orig_execute_ex(execute_data);
      return;
    }

    if (!execute_data->prev_execute_data ||
        !execute_data->prev_execute_data->func ||
        !ZEND_USER_CODE(execute_data->prev_execute_data->func->type) ||
        !execute_data->prev_execute_data->opline) {
      should_disable_ht(execute_data, function_name, NULL, NULL,
                        config_disabled_functions_reg,
                        config_disabled_functions);
    } else if ((execute_data->prev_execute_data->opline->opcode ==
                    ZEND_DO_FCALL ||
                execute_data->prev_execute_data->opline->opcode ==
                    ZEND_DO_UCALL ||
                execute_data->prev_execute_data->opline->opcode ==
                    ZEND_DO_ICALL ||
                execute_data->prev_execute_data->opline->opcode ==
                    ZEND_DO_FCALL_BY_NAME)) {
      should_disable_ht(execute_data, function_name, NULL, NULL,
                        config_disabled_functions_reg,
                        config_disabled_functions);
    }

    // When a function's return value isn't used, php doesn't store it in the
    // execute_data, so we need to use a local variable to be able to match on
    // it later.
    if (EX(return_value) == NULL) {
      memset(&ret_val, 0, sizeof(ret_val));
      EX(return_value) = &ret_val;
    }

    orig_execute_ex(execute_data);

    should_drop_on_ret_ht(
        EX(return_value), function_name,
        SNUFFLEUPAGUS_G(config)
            .config_disabled_functions_reg_ret->disabled_functions,
        SNUFFLEUPAGUS_G(config).config_disabled_functions_ret, execute_data);
    efree(function_name);

    if (EX(return_value) == &ret_val) {
      EX(return_value) = NULL;
    }
  } else {
    orig_execute_ex(execute_data);
  }
}

static void sp_zend_execute_internal(INTERNAL_FUNCTION_PARAMETERS) {
  is_in_eval_and_whitelisted(execute_data);

  if (UNEXPECTED(NULL != orig_zend_execute_internal)) {
    // LCOV_EXCL_START
    orig_zend_execute_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    // LCOV_EXCL_STOP
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

  zend_string *zend_filename = zend_string_init(filename, strlen(filename), 0);
  const HashTable *disabled_functions_hooked =
      SNUFFLEUPAGUS_G(config).config_disabled_functions_hooked;

  switch (data->opline->opcode) {
    case ZEND_INCLUDE_OR_EVAL:
      if (true == SNUFFLEUPAGUS_G(config).config_readonly_exec->enable) {
        terminate_if_writable(filename);
      }
      switch (data->opline->extended_value) {
        case ZEND_INCLUDE:
          is_builtin_matching(
              zend_filename, "include", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, "include",
                                     sizeof("include") - 1),
              disabled_functions_hooked);
          break;
        case ZEND_REQUIRE:
          is_builtin_matching(
              zend_filename, "require", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, "require",
                                     sizeof("require") - 1),
              disabled_functions_hooked);
          break;
        case ZEND_REQUIRE_ONCE:
          is_builtin_matching(
              zend_filename, "require_once", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, "require_once",
                                     sizeof("require_once") - 1),
              disabled_functions_hooked);
          break;
        case ZEND_INCLUDE_ONCE:
          is_builtin_matching(
              zend_filename, "include_once", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, "include_once",
                                     sizeof("include_once") - 1),
              disabled_functions_hooked);
          break;
          EMPTY_SWITCH_DEFAULT_CASE();  // LCOV_EXCL_LINE
      }
  }
  efree(zend_filename);

end:
  return orig_zend_stream_open(filename, handle);
}

int hook_execute(void) {
  TSRMLS_FETCH();

  if (NULL == orig_execute_ex && NULL == orig_zend_stream_open) {
    /* zend_execute_ex is used for "user" function calls */
    orig_execute_ex = zend_execute_ex;
    zend_execute_ex = sp_execute_ex;

    /* zend_execute_internal is used for "builtin" functions calls */
    orig_zend_execute_internal = zend_execute_internal;
    zend_execute_internal = sp_zend_execute_internal;

    /* zend_stream_open_function is used for include-related stuff */
    orig_zend_stream_open = zend_stream_open_function;
    zend_stream_open_function = sp_stream_open;
  }

  return SUCCESS;
}
