#include "php_snuffleupagus.h"

static void should_disable(zend_execute_data* execute_data,
                           const char* complete_function_path,
                           const zend_string* builtin_param,
                           const char* builtin_param_name,
                           const sp_list_node* config,
                           const zend_string* current_filename);

static void should_drop_on_ret(const zval* return_value,
                               const sp_list_node* config,
                               const char* complete_function_path,
                               zend_execute_data* execute_data);

char* get_complete_function_path(zend_execute_data const* const execute_data) {
  if (!execute_data) {
    return NULL;  // LCOV_EXCL_LINE
  }
  const zend_function *const func = execute_data->func;
  if (!(func->common.function_name)) {
    return NULL;
  }

  char const* const function_name = ZSTR_VAL(func->common.function_name);
  char* complete_path_function = NULL;

  if ((func->type == ZEND_USER_FUNCTION || func->type == ZEND_INTERNAL_FUNCTION) && func->common.scope) {
    const char *const class_name = ZSTR_VAL(func->common.scope->name);
    const size_t len = strlen(class_name) + 2 + strlen(function_name) + 1;
    complete_path_function = emalloc(len);
    snprintf(complete_path_function, len, "%s::%s", class_name, function_name);
  } else {
    complete_path_function = estrdup(function_name);
  }

  return complete_path_function;
}

static bool is_functions_list_matching(zend_execute_data* execute_data,
                                       sp_list_node* functions_list) {
  zend_execute_data *orig_execute_data, *current;
  orig_execute_data = current = execute_data;
  sp_list_node const* it = functions_list;

  while (current) {
    if (it == NULL) {  // every function in the list matched, we've got a match!
      EG(current_execute_data) = orig_execute_data;
      return true;
    }

    EG(current_execute_data) = current;

    char* const complete_path_function = get_complete_function_path(current);
    if (!complete_path_function) {
      break;
    }
    int match = strcmp(((char*)it->data), complete_path_function);
    efree(complete_path_function);

    if (0 == match) {
      it = it->next;
    }
    current = current->prev_execute_data;
  }

  EG(current_execute_data) = orig_execute_data;
  return false;
}

static bool is_local_var_matching(zend_execute_data* execute_data, const sp_disabled_function* const config_node) {
  zval* var_value = {0};

  var_value = sp_get_var_value(execute_data, config_node->var, false);
  if (var_value) {
    if (Z_TYPE_P(var_value) == IS_ARRAY) {
      if (config_node->key || config_node->r_key) {
        if (sp_match_array_key(var_value, config_node->key, config_node->r_key)) {
          return true;
        }
      } else if (sp_match_array_value(var_value, config_node->value, config_node->r_value)) {
        return true;
      }
    } else {
      zend_string const* const var_value_str = sp_zval_to_zend_string(var_value);
      bool match = sp_match_value(var_value_str, config_node->value, config_node->r_value);

      if (true == match) {
        return true;
      }
    }
  }
  return false;
}

static inline const char* get_fn_arg_name(zend_function *fn, uint32_t i) {
  if (fn->type == ZEND_USER_FUNCTION || (fn->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
    return ZSTR_VAL(fn->op_array.arg_info[i].name);
  } else {
    return fn->internal_function.arg_info[i].name;
  }
}

static bool is_param_matching(zend_execute_data* execute_data,
                              sp_disabled_function const* const config_node,
                              const zend_string* builtin_param,
                              const char* builtin_param_name,
                              const char** arg_name,
                              const zend_string** arg_value_str) {
  // builtin functions
  if (builtin_param) {
    /* We're matching on a language construct (here named "builtin"),
    * and they can only take a single argument, but PHP considers them
    * differently than functions arguments. */
    *arg_name = builtin_param_name;
    *arg_value_str = builtin_param;
    return sp_match_value(builtin_param, config_node->value,
                          config_node->r_value);
  }

  // safeguards
  if (!execute_data || !execute_data->func) {
    sp_log_debug("no execute data -> silently ignore parameter matching");
    return false;
  }

  *arg_name = NULL;
  int call_num_args = EX_NUM_ARGS();
  zend_function *fn = execute_data->func;
  int fn_num_args = fn->common.num_args;

  if (!call_num_args) {
    sp_log_debug("no call arguments -> return");
    return false; // no arguments to check
  }

  if (config_node->pos > call_num_args - 1 || config_node->pos > fn_num_args) {
    // trying to match argument beyond last given argument OR beyond last declared argument.
    // this is perfectly normal for functions with
    //   (a) optional arguments
    //   (b) excess arguments
    //   (c) variadic arguments which are not supported
    return false;
  }

  zval* arg_value = NULL;

  if (config_node->pos > -1) {
    if (config_node->pos < fn_num_args) {
      *arg_name = get_fn_arg_name(fn, config_node->pos);
    }
    arg_value = ZEND_CALL_ARG(execute_data, config_node->pos + 1);
  } else if (config_node->param) {
    *arg_name = config_node->param->value;
    arg_value = sp_get_var_value(execute_data, config_node->param, true);
  } else if (config_node->r_param) {
    for (int i = 0; i < call_num_args; i++) {
      *arg_name = get_fn_arg_name(fn, i);
      if (true == sp_is_regexp_matching(config_node->r_param, *arg_name)) {
        arg_value = ZEND_CALL_ARG(execute_data, i + 1);
      }
    }
  }
  
  if (!arg_value) {
    sp_log_debug("no argument match -> return");
    return false;
  }

  if (config_node->param_type) {
    if (config_node->param_type == Z_TYPE_P(arg_value)) {
      if (!(config_node->key || config_node->r_key || config_node->value || config_node->r_value)) { // Are we matching on the `type` only?
        sp_log_debug("arg type match only.");
        return true;
      }
    } else {
      sp_log_debug("arg type mismatch -> return");
      return false;
    }
  }

  *arg_value_str = sp_zval_to_zend_string(arg_value);
  if (Z_TYPE_P(arg_value) == IS_ARRAY) {
    if (config_node->key || config_node->r_key) {
      if (sp_match_array_key(arg_value, config_node->key, config_node->r_key)) {
        return true;
      }
    } else if (sp_match_array_value(arg_value, config_node->value, config_node->r_value)) {
        return true;
    }
  } else if (sp_match_value(*arg_value_str, config_node->value, config_node->r_value)) {
    return true;
  }

  return false;
}

static zend_execute_data* is_file_matching(
    zend_execute_data* const execute_data,
    sp_disabled_function const* const config_node,
    zend_string const* const current_filename) {
#define ITERATE(ex)                                            \
  ex = ex->prev_execute_data;                                  \
  while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) \
    ex = ex->prev_execute_data;                                \
  if (!ex) return NULL;

  zend_execute_data* ex = execute_data;
  if (config_node->filename) {
    if (sp_zend_string_equals(current_filename, config_node->filename)) {
      return ex;  // LCOV_EXCL_LINE
    }
    ITERATE(ex);
    if (zend_string_equals(ex->func->op_array.filename, config_node->filename)) {
      return ex;  // LCOV_EXCL_LINE
    }
  } else if (config_node->r_filename) {
    if (sp_is_regexp_matching_zstr(config_node->r_filename, current_filename)) {
      return ex;
    }
    ITERATE(ex);
    if (sp_is_regexp_matching_zstr(config_node->r_filename, ex->func->op_array.filename)) {
      return ex;
    }
  }
  return NULL;
#undef ITERATE
}

inline static bool check_is_builtin_name(
    sp_disabled_function const* const config_node) {
  if (EXPECTED(config_node->function)) {
    return (zend_string_equals_literal(config_node->function, "include") ||
            zend_string_equals_literal(config_node->function, "include_once") ||
            zend_string_equals_literal(config_node->function, "require") ||
            zend_string_equals_literal(config_node->function, "require_once") ||
            zend_string_equals_literal(config_node->function, "echo"));
  }
  return false;  // LCOV_EXCL_LINE
}

void should_disable_ht(zend_execute_data* execute_data,
                       const char* function_name,
                       const zend_string* builtin_param,
                       const char* builtin_param_name,
                       const sp_list_node* config, const HashTable* ht) {
  const sp_list_node* ht_entry = NULL;
  zend_string* current_filename;

  if (!execute_data) {
    return;  // LCOV_EXCL_LINE
  }

  if (UNEXPECTED(builtin_param && !strcmp(function_name, "eval"))) {
    current_filename = get_eval_filename(zend_get_executed_filename());
  } else {
    const char* tmp = zend_get_executed_filename();
    current_filename = zend_string_init(tmp, strlen(tmp), 0);
  }

  ht_entry = zend_hash_str_find_ptr(ht, VAR_AND_LEN(function_name));

  if (ht_entry) {
    should_disable(execute_data, function_name, builtin_param,
                   builtin_param_name, ht_entry, current_filename);
  } else if (config && config->data) {
    should_disable(execute_data, function_name, builtin_param,
                   builtin_param_name, config, current_filename);
  }

  efree(current_filename);
}

static void should_disable(zend_execute_data* execute_data,
                           const char* complete_function_path,
                           const zend_string* builtin_param,
                           const char* builtin_param_name,
                           const sp_list_node* config,
                           const zend_string* current_filename) {
  char current_file_hash[SHA256_SIZE * 2 + 1] = {0};

  while (config) {
    sp_disabled_function const* const config_node = (sp_disabled_function*)(config->data);
    const char* arg_name = NULL;
    const zend_string* arg_value_str = NULL;

    /* The order matters, since when we have `config_node->functions_list`,
    we also do have `config_node->function` */
    if (config_node->functions_list) {
      if (false == is_functions_list_matching(execute_data, config_node->functions_list)) {
        goto next;
      }
    } else if (config_node->function) {
      if (0 != strcmp(ZSTR_VAL(config_node->function), complete_function_path)) {
        goto next;  // LCOV_EXCL_LINE
      }
    } else if (config_node->r_function) {
      if (false == sp_is_regexp_matching(config_node->r_function, complete_function_path)) {
        goto next;
      }
    }

    if (config_node->line) {
      if (config_node->line != zend_get_executed_lineno()) {
        goto next;
      }
    }

    if (config_node->filename || config_node->r_filename) {
      zend_execute_data* ex = is_file_matching(execute_data, config_node, current_filename);
      if (!ex) {
        goto next;
      }
    }

    if (config_node->cidr) {
      const char* client_ip = get_ipaddr();
      if (client_ip && false == cidr_match(client_ip, config_node->cidr)) {
        goto next;
      }
    }

    if (config_node->var) {
      if (false == is_local_var_matching(execute_data, config_node)) {
        goto next;
      }
    }

    if (config_node->hash) {
      if ('\0' == current_file_hash[0]) {
        compute_hash(ZSTR_VAL(current_filename), current_file_hash);
      }
      if (0 != strncmp(current_file_hash, ZSTR_VAL(config_node->hash), SHA256_SIZE)) {
        goto next;
      }
    }

    /* Check if we filter on parameter value*/
    if (config_node->param || config_node->r_param ||
        (config_node->pos != -1)  ||
        ((config_node->r_value || config_node->value) && !config_node->var)) {
      if (!builtin_param &&
#if PHP_VERSION_ID >= 80000
          ZEND_ARG_IS_VARIADIC(execute_data->func->op_array.arg_info)
#else
          execute_data->func->op_array.arg_info->is_variadic
#endif
      ) {
        sp_log_warn(
            "disable_function",
            "Snuffleupagus doesn't support variadic functions yet, sorry. "
            "Check https://github.com/jvoisin/snuffleupagus/issues/164 for "
            "details.");
      } else if (false == is_param_matching(execute_data, config_node, builtin_param, builtin_param_name, &arg_name, &arg_value_str)) {
        goto next;
      }
    }

    /* Everything matched.*/
    if (true == config_node->allow) {
      return;
    }

    if (config_node->functions_list) {
      sp_log_disable(ZSTR_VAL(config_node->function), arg_name, arg_value_str, config_node);
    } else {
      sp_log_disable(complete_function_path, arg_name, arg_value_str, config_node);
    }

  next:
    config = config->next;
  }
}

void should_drop_on_ret_ht(const zval* return_value, const char* function_name,
                           const sp_list_node* config, const HashTable* ht,
                           zend_execute_data* execute_data) {
  const sp_list_node* ht_entry = NULL;

  if (!function_name) {
    return;  // LCOV_EXCL_LINE
  }

  ht_entry = zend_hash_str_find_ptr(ht, function_name, strlen(function_name));

  if (ht_entry) {
    should_drop_on_ret(return_value, ht_entry, function_name, execute_data);
  } else if (config && config->data) {
    should_drop_on_ret(return_value, config, function_name, execute_data);
  }
}

static void should_drop_on_ret(const zval* return_value,
                               const sp_list_node* config,
                               const char* complete_function_path,
                               zend_execute_data* execute_data) {
  const char* current_filename = zend_get_executed_filename(TSRMLS_C);
  char current_file_hash[SHA256_SIZE * 2 + 1] = {0};
  bool match_type = false, match_value = false;

  while (config) {
    sp_disabled_function const* const config_node =
        (sp_disabled_function*)(config->data);

    assert(config_node->function || config_node->r_function);

    if (config_node->functions_list) {
      if (false == is_functions_list_matching(execute_data,
                                              config_node->functions_list)) {
        goto next;
      }
    } else if (config_node->function) {
      if (0 !=
          strcmp(ZSTR_VAL(config_node->function), complete_function_path)) {
        goto next;  // LCOV_EXCL_LINE
      }
    } else if (config_node->r_function) {
      if (false == sp_is_regexp_matching(config_node->r_function,
                                         complete_function_path)) {
        goto next;
      }
    }

    if (config_node->filename) { /* Check the current file name. */
      if (0 != strcmp(current_filename, ZSTR_VAL(config_node->filename))) {
        goto next;
      }
    } else if (config_node->r_filename) {
      if (false ==
          sp_is_regexp_matching(config_node->r_filename, current_filename)) {
        goto next;
      }
    }

    if (config_node->hash) {
      if ('\0' == current_file_hash[0]) {
        compute_hash(current_filename, current_file_hash);
      }
      if (0 != strncmp(current_file_hash, ZSTR_VAL(config_node->hash),
                       SHA256_SIZE)) {
        goto next;
      }
    }

    const zend_string* ret_value_str = NULL;
    sp_php_type ret_type = SP_PHP_TYPE_NULL;

    if (return_value) {
      ret_value_str = sp_zval_to_zend_string(return_value);
      ret_type = Z_TYPE_P(return_value);
    }

    match_type = (config_node->ret_type) &&
                 (config_node->ret_type == ret_type);
    match_value = return_value && (config_node->ret || config_node->r_ret) &&
                  (true == sp_match_value(ret_value_str, config_node->ret, config_node->r_ret));

    if (true == match_type || true == match_value) {
      if (true == config_node->allow) {
        return;
      }
      sp_log_disable_ret(complete_function_path, ret_value_str, config_node);
    }
  next:
    config = config->next;
  }
}

ZEND_FUNCTION(check_disabled_function) {
  zif_handler orig_handler;
  const char* current_function_name = get_active_function_name();

  should_disable_ht(execute_data, current_function_name, NULL, NULL, SPCFG(disabled_functions_reg).disabled_functions, SPCFG(disabled_functions_hooked));

  orig_handler = zend_hash_str_find_ptr(SPG(disabled_functions_hook), VAR_AND_LEN(current_function_name));
  orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

  should_drop_on_ret_ht(return_value, current_function_name, SPCFG(disabled_functions_reg_ret).disabled_functions, SPCFG(disabled_functions_ret_hooked), execute_data);
}

static int hook_functions_regexp(const sp_list_node* config) {
  while (config && config->data) {
    const zend_string* function_name = ((sp_disabled_function*)config->data)->function;
    sp_regexp *function_name_sp_regexp = ((sp_disabled_function*)config->data)->r_function;
    const sp_pcre* function_name_regexp = function_name_sp_regexp ? function_name_sp_regexp->re : NULL;

    assert(function_name || function_name_regexp);

    if (function_name) {
      HOOK_FUNCTION(ZSTR_VAL(function_name), disabled_functions_hook, PHP_FN(check_disabled_function));
    } else {
      HOOK_FUNCTION_BY_REGEXP(function_name_regexp, disabled_functions_hook, PHP_FN(check_disabled_function));
    }

    config = config->next;
  }
  return SUCCESS;
}

static void hook_functions(HashTable* to_hook_ht, HashTable* hooked_ht) {
  zend_string* key;
  zval* value;

  ZEND_HASH_FOREACH_STR_KEY_VAL(to_hook_ht, key, value) {
    bool hooked = HOOK_FUNCTION(ZSTR_VAL(key), disabled_functions_hook, PHP_FN(check_disabled_function));
    bool is_builtin = check_is_builtin_name(((sp_list_node*)Z_PTR_P(value))->data);
    if (hooked || is_builtin) {
      zend_symtable_add_new(hooked_ht, key, value);
      zend_hash_del(to_hook_ht, key);
    }
  }
  ZEND_HASH_FOREACH_END();
}

ZEND_FUNCTION(eval_blacklist_callback) {
  zif_handler orig_handler;
  char* current_function_name = get_complete_function_path(EG(current_execute_data));

  if (!current_function_name) {
    return;
  }

  if( true == check_is_in_eval_whitelist(current_function_name)) {
    goto whitelisted;
  }

  if (SPG(in_eval) > 0) {
    const sp_config_eval* config_eval = &(SPCFG(eval));

    if (config_eval->dump) {
      sp_log_request(config_eval->dump, config_eval->textual_representation);
    }
    if (config_eval->simulation) {
      sp_log_simulation("eval", "A call to '%s' was tried in eval. logging it.", current_function_name);
    } else {
      sp_log_drop("eval", "A call to '%s' was tried in eval. dropping it.", current_function_name);
    }
  }

whitelisted:

  orig_handler = zend_hash_str_find_ptr(SPG(sp_eval_blacklist_functions_hook), current_function_name, strlen(current_function_name));
  efree(current_function_name);
  orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

int hook_disabled_functions(void) {
  TSRMLS_FETCH();

  int ret = SUCCESS;

  hook_functions(SPCFG(disabled_functions), SPCFG(disabled_functions_hooked));
  hook_functions(SPCFG(disabled_functions_ret), SPCFG(disabled_functions_ret_hooked));

  ret |= hook_functions_regexp(SPCFG(disabled_functions_reg).disabled_functions);

  ret |= hook_functions_regexp(SPCFG(disabled_functions_reg_ret).disabled_functions);

  if (NULL != SPCFG(eval).blacklist) {
    sp_list_node* it = SPCFG(eval).blacklist;

    while (it) {
      hook_function(ZSTR_VAL((zend_string*)it->data),
                    SPG(sp_eval_blacklist_functions_hook),
                    PHP_FN(eval_blacklist_callback));
      it = it->next;
    }
  }
  return ret;
}

zend_write_func_t zend_write_default = NULL;

#if PHP_VERSION_ID >= 80000
size_t hook_echo(const char* str, size_t str_length) {
#else
int hook_echo(const char* str, size_t str_length) {
#endif
  zend_string* zs = zend_string_init(str, str_length, 0);

  should_disable_ht(EG(current_execute_data), "echo", zs, NULL, SPCFG(disabled_functions_reg).disabled_functions, SPCFG(disabled_functions_hooked));

  zend_string_release(zs);

  return zend_write_default(str, str_length);
}
