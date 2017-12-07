#include "php_snuffleupagus.h"

#include "zend_execute.h"
#include "zend_hash.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static char* get_complete_function_path(
    zend_execute_data const* const execute_data) {
  if (!(execute_data->func->common.function_name)) {
    return NULL;
  }

  char const* class_name;
  char const* const function_name =
      ZSTR_VAL(execute_data->func->common.function_name);
  char* complete_path_function = NULL;

  class_name = get_active_class_name(NULL);
  if (*class_name) {
    const size_t len = strlen(class_name) + 2 + strlen(function_name) + 1;
    complete_path_function = emalloc(len);
    snprintf(complete_path_function, len, "%s::%s", class_name, function_name);
  } else {
    complete_path_function = estrdup(function_name);
  }
  return complete_path_function;
}

static bool is_functions_list_matching(zend_execute_data* execute_data,
                                       sp_node_t* functions_list) {
  zend_execute_data *orig_execute_data, *current;
  orig_execute_data = current = execute_data;
  sp_node_t* it = functions_list;

  while (current) {
    if (it == NULL) {  // every function in the list matched, we've got a match!
      EG(current_execute_data) = orig_execute_data;
      return true;
    }

    EG(current_execute_data) = current;

    char* complete_path_function = get_complete_function_path(current);
    if (!complete_path_function) {
      goto end;
    }
    int match = strcmp(((char*)it->data), complete_path_function);
    efree(complete_path_function);

    if (0 == match) {
      it = it->next;
      current = current->prev_execute_data;
    } else {
      goto end;
    }
  }

end:
  EG(current_execute_data) = orig_execute_data;
  return false;
}

static bool is_local_var_matching(
    zend_execute_data* execute_data,
    const sp_disabled_function* const config_node) {
  char *var_value_str;
  bool ret = false;

  var_value_str = get_value(execute_data, config_node->var);
  if (var_value_str) {
    ret = (true == sp_match_value(var_value_str, config_node->value,
				  config_node->value_r));
    efree(var_value_str);
  }
  return ret;
}

static const sp_node_t* get_config_node(const char* builtin_name) {
  if (!builtin_name) {
    return SNUFFLEUPAGUS_G(config)
        .config_disabled_functions->disabled_functions;
  } else if (!strcmp(builtin_name, "eval")) {
    return SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_eval;
  } else if (!strcmp(builtin_name, "include") ||
             !strcmp(builtin_name, "include_once") ||
             !strcmp(builtin_name, "require") ||
             !strcmp(builtin_name, "require_once")) {
    return SNUFFLEUPAGUS_G(config)
        .config_disabled_constructs->construct_include;
  }
  return NULL;  // This should never happen.
}

static bool is_param_matching(zend_execute_data* execute_data,
                              sp_disabled_function const* const config_node,
                              const char* builtin_name,
                              const char* builtin_param, const char** arg_name,
                              const char* builtin_param_name,
                              const char** arg_value_str) {
  int nb_param = execute_data->func->common.num_args;
  int i = 0;

  if (config_node->pos != -1) {
    if (config_node->pos <= nb_param) {
      char* complete_function_path = get_complete_function_path(execute_data);
      sp_log_err("config",
                 "It seems that you wrote a rule filtering on the "
                 "%d%s argument of the function '%s', but it takes only %d "
                 "arguments. "
                 "Matching on _all_ arguments instead.",
                 config_node->pos, GET_SUFFIX(config_node->pos),
                 complete_function_path, nb_param);
      efree(complete_function_path);
    } else {
      i = config_node->pos;
      nb_param = (config_node->pos) + 1;
    }
  }

  if (builtin_name) {
    /* We're matching on a language construct (here named "builtin"),
     * and they can only take a single argument, but PHP considers them
     * differently than functions arguments. */
    *arg_name = builtin_param_name;
    *arg_value_str = builtin_param;
    return sp_match_value(builtin_param, config_node->value,
                          config_node->value_r);
  } else {
    // We're matching on a function (and not a language construct)
    for (; i < nb_param; i++) {
      if (ZEND_USER_CODE(execute_data->func->type)) {  // yay consistency
        *arg_name = ZSTR_VAL(execute_data->func->common.arg_info[i].name);
      } else {
        *arg_name = execute_data->func->internal_function.arg_info[i].name;
      }

      const bool arg_matching =
          config_node->param && (0 == strcmp(*arg_name, config_node->param));
      const bool pcre_matching =
          config_node->r_param &&
          (true == is_regexp_matching(config_node->r_param, *arg_name));

      /* This is the parameter name we're looking for. */
      if (true == arg_matching || true == pcre_matching ||
          (config_node->pos != -1)) {
        zval* arg_value = ZEND_CALL_VAR_NUM(execute_data, i);

        if (config_node->param_type) {  // Are we matching on the `type`?
          if (config_node->param_type == Z_TYPE_P(arg_value)) {
            return true;
          }
        } else if (Z_TYPE_P(arg_value) == IS_ARRAY) {
          *arg_value_str = estrdup("Array");
          // match on arr -> match on all key content, if a key is an array,
          // ignore it
          // match on arr[foo] -> match only on key foo, if the key is an
          // array, match on all keys content
          if (config_node->param_is_array == true) {
            if (true == sp_match_array_key_recurse(
                            arg_value, config_node->param_array_keys,
                            config_node->value, config_node->value_r)) {
              return true;
            }
          } else {  // match on all keys, but don't go into subarray
            if (true == sp_match_array_key(arg_value, config_node->value,
                                           config_node->value_r)) {
              return true;
            }
          }
        } else {
          *arg_value_str = sp_convert_to_string(arg_value);
          if (true == sp_match_value(*arg_value_str, config_node->value,
                                     config_node->value_r)) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool should_disable(zend_execute_data* execute_data, const char* builtin_name,
                    const char* builtin_param, const char* builtin_param_name) {
  char current_file_hash[SHA256_SIZE * 2 + 1] = {0};
  const sp_node_t* config = get_config_node(builtin_name);
  char* complete_path_function = get_complete_function_path(execute_data);
  char const* client_ip = sp_getenv("REMOTE_ADDR");
  const char* current_filename;

  if (!config || !config->data) {
    return false;
  }

  if (builtin_name && !strcmp(builtin_name, "eval")) {
    current_filename = get_eval_filename(zend_get_executed_filename());
  } else {
    current_filename = zend_get_executed_filename();
  }

  if (!complete_path_function) {
    if (builtin_name) {
      complete_path_function = (char*)builtin_name;
    } else {
      return false;
    }
  }

  while (config) {
    sp_disabled_function const* const config_node =
        (sp_disabled_function*)(config->data);
    const char* arg_name = NULL;
    const char* arg_value_str = NULL;

    /* The order matters, since when we have `config_node->functions_list`,
    we also do have `config_node->function` */
    if (config_node->functions_list) {
      if (false == is_functions_list_matching(execute_data,
                                              config_node->functions_list)) {
        goto next;
      }
    } else if (config_node->function) {
      if (0 != strcmp(config_node->function, complete_path_function)) {
        goto next;
      }
    } else if (config_node->r_function) {
      if (false ==
          is_regexp_matching(config_node->r_function, complete_path_function)) {
        goto next;
      }
    }

    if (config_node->line) {
      if (config_node->line != zend_get_executed_lineno()) {
        goto next;
      }
    }

    if (config_node->filename) { /* Check the current file name. */
      if (0 != strcmp(current_filename, config_node->filename)) {
        goto next;
      }
    } else if (config_node->r_filename) {
      if (false ==
          is_regexp_matching(config_node->r_filename, current_filename)) {
        goto next;
      }
    }

    if (client_ip && config_node->cidr &&
        (false == cidr_match(client_ip, config_node->cidr))) {
      goto next;
    }

    if (config_node->var) {
      if (false == is_local_var_matching(execute_data, config_node)) {
        goto next;
      }
    }

    if (config_node->hash) {
      if ('\0' == current_file_hash[0]) {
        compute_hash(current_filename, current_file_hash);
      }
      if (0 != strncmp(current_file_hash, config_node->hash, SHA256_SIZE)) {
        goto next;
      }
    }

    /* Check if we filter on parameter value*/
    if (config_node->param || config_node->r_param ||
        (config_node->pos != -1)) {
      if (false == is_param_matching(execute_data, config_node, builtin_name,
                                     builtin_param, &arg_name,
                                     builtin_param_name, &arg_value_str)) {
        goto next;
      }
    }

    /* Everything matched.*/

    if (true == config_node->allow) {
      goto allow;
    }

    if (config_node->functions_list) {
      sp_log_disable(config_node->function, arg_name, arg_value_str,
                     config_node);
    } else {
      sp_log_disable(complete_path_function, arg_name, arg_value_str,
                     config_node);
    }
    if (true == config_node->simulation) {
      goto next;
    } else {  // We've got a match, the function won't be executed
      if (builtin_name == NULL) {
        efree(complete_path_function);
      }
      return true;
    }
  next:
    config = config->next;
  }
allow:
  if (builtin_name == NULL) {
    efree(complete_path_function);
  }
  return false;
}

static bool should_drop_on_ret(zval* return_value,
                               const zend_execute_data* const execute_data) {
  const sp_node_t* config =
      SNUFFLEUPAGUS_G(config).config_disabled_functions_ret->disabled_functions;
  char* complete_path_function = get_complete_function_path(execute_data);
  const char* current_filename = zend_get_executed_filename(TSRMLS_C);
  char current_file_hash[SHA256_SIZE * 2 + 1] = {0};
  bool match_type = false, match_value = false;

  if (!complete_path_function) {
    return false;
  }

  if (!config || !config->data) {
    return false;
  }

  while (config) {
    char* ret_value_str = NULL;
    sp_disabled_function const* const config_node =
        (sp_disabled_function*)(config->data);

    if (config_node->function) {
      if (0 != strcmp(config_node->function, complete_path_function)) {
        goto next;
      }
    } else if (config_node->r_function) {
      if (false ==
          is_regexp_matching(config_node->r_function, complete_path_function)) {
        goto next;
      }
    }

    if (config_node->filename) { /* Check the current file name. */
      if (0 != strcmp(current_filename, config_node->filename)) {
        goto next;
      }
    } else if (config_node->r_filename) {
      if (false ==
          is_regexp_matching(config_node->r_filename, current_filename)) {
        goto next;
      }
    }

    if (config_node->hash) {
      if ('\0' == current_file_hash[0]) {
        compute_hash(current_filename, current_file_hash);
      }
      if (0 != strncmp(current_file_hash, config_node->hash, SHA256_SIZE)) {
        goto next;
      }
    }

    ret_value_str = sp_convert_to_string(return_value);

    match_type = (config_node->ret_type) &&
                 (config_node->ret_type == Z_TYPE_P(return_value));
    match_value = (config_node->ret || config_node->r_ret) &&
                  (true == sp_match_value(ret_value_str, config_node->ret,
                                          config_node->r_ret));

    if (true == match_type || true == match_value) {
      if (true == config_node->allow) {
        efree(complete_path_function);
        efree(ret_value_str);
        return false;
      }
      sp_log_disable_ret(complete_path_function, ret_value_str, config_node);
      if (false == config_node->simulation) {
        efree(complete_path_function);
        efree(ret_value_str);
        return true;
      }
    }
  next:
    efree(ret_value_str);
    config = config->next;
  }
  efree(complete_path_function);
  return false;
}

ZEND_FUNCTION(check_disabled_function) {
  void (*orig_handler)(INTERNAL_FUNCTION_PARAMETERS);
  const char* current_function_name = get_active_function_name(TSRMLS_C);

  if (true == should_disable(execute_data, NULL, NULL, NULL)) {
    sp_terminate();
  }

  if ((orig_handler = zend_hash_str_find_ptr(
           SNUFFLEUPAGUS_G(disabled_functions_hook), current_function_name,
           strlen(current_function_name)))) {
    orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    if (true == should_drop_on_ret(return_value, execute_data)) {
      sp_terminate();
    }
  } else {
    sp_log_err(
        "disabled_functions",
        "Unable to find the pointer to the original function '%s' in the "
        "hashtable.\n",
        current_function_name);
  }
}

static int hook_functions(const sp_node_t* config) {
  while (config && config->data) {
    const char* function_name = ((sp_disabled_function*)config->data)->function;
    const pcre* function_name_regexp =
        ((sp_disabled_function*)config->data)->r_function;

    if (NULL != function_name) {  // hook function by name
      HOOK_FUNCTION(function_name, disabled_functions_hook,
                    PHP_FN(check_disabled_function), false);
    } else if (NULL != function_name_regexp) {  // hook function by regexp
      HOOK_FUNCTION_BY_REGEXP(function_name_regexp, disabled_functions_hook,
                              PHP_FN(check_disabled_function), false);
    } else {
      return FAILURE;
    }

    config = config->next;
  }
  return SUCCESS;
}

int hook_disabled_functions(void) {
  TSRMLS_FETCH();

  int ret = SUCCESS;

  ret |= hook_functions(
      SNUFFLEUPAGUS_G(config).config_disabled_functions->disabled_functions);
  ret |= hook_functions(SNUFFLEUPAGUS_G(config)
                            .config_disabled_functions_ret->disabled_functions);

  return ret;
}
