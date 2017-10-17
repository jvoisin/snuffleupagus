#include "php_snuffleupagus.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)


static int get_construct_type(sp_disabled_function const *const df) {
  const struct {
    unsigned int type;
    char *keys[5];
  } mapping[] = {
    {
      .type = ZEND_INCLUDE_OR_EVAL,
      .keys = {"include", "include_once", "require", "require_once", NULL}
    },{
      .type = ZEND_ECHO,
      .keys = {"echo", NULL}
    },{
      .type = ZEND_NEW,
      .keys = {"new", NULL}
    },{
      .type = ZEND_EXIT,
      .keys = {"exit", NULL}
    },{
      .type = ZEND_STRLEN,
      .keys = {"strlen", NULL}
    },{
      .type = 0,
      .keys = {NULL}
   }
  };

  // FIXME: This can be optimized
  // FIXME the ->function and r_fonction tests are _wrong_
  for (size_t i=0; 0 != mapping[i].type; i++) {
    for (size_t j=0; NULL != mapping[i].keys[j]; j++) {
      if (df->function) {
        if (0 == strcmp(df->function, mapping[i].keys[j])) {
          return mapping[i].type;
        }
      } else if (df->r_function) {
        if (true == is_regexp_matching(df->r_function, mapping[i].keys[j])) {
          return mapping[i].type;
        }
      }
    }
  }
  return -1;
}

static int parse_enable(char *line, bool * restrict retval, bool * restrict simulation) {
  bool enable = false, disable = false;
  sp_config_functions sp_config_funcs[] = {
    {parse_empty, SP_TOKEN_ENABLE, &(enable)},
    {parse_empty, SP_TOKEN_DISABLE, &(disable)},
    {parse_empty, SP_TOKEN_SIMULATION, simulation},
    {0}};

  int ret = parse_keywords(sp_config_funcs, line);

  if (0 != ret) {
    return ret;
  }

  if (!(enable ^ disable)) {
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu.", sp_line_no);
    return -1;
  }

  *retval = enable;

  return ret;
}

int parse_random(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_random->enable), NULL);
}

int parse_disable_xxe(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_disable_xxe->enable), NULL);
}

int parse_auto_cookie_secure(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_auto_cookie_secure->enable), NULL);
}

int parse_global_strict(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_global_strict->enable), NULL);
}

int parse_unserialize(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_unserialize->enable), &(SNUFFLEUPAGUS_G(config).config_unserialize->simulation));
}

int parse_readonly_exec(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_readonly_exec->enable), &(SNUFFLEUPAGUS_G(config).config_readonly_exec->simulation));
}

int parse_global(char *line) {
  sp_config_functions sp_config_funcs_global[] = {
      {parse_str, SP_TOKEN_ENCRYPTION_KEY,
       &(SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key)},
      {parse_str, SP_TOKEN_ENV_VAR,
       &(SNUFFLEUPAGUS_G(config).config_snuffleupagus->cookies_env_var)},
      {0}};
  return parse_keywords(sp_config_funcs_global, line);
}

int parse_cookie_encryption(char *line) {
  int ret = 0;
  char *name = NULL;

  sp_config_functions sp_config_funcs_cookie_encryption[] = {
      {parse_str, SP_TOKEN_NAME, &name},
      {0}};

  ret = parse_keywords(sp_config_funcs_cookie_encryption, line);
  if (0 != ret) {
    return ret;
  }

  if (0 == (SNUFFLEUPAGUS_G(config).config_snuffleupagus->cookies_env_var)) {
    sp_log_err("config", "You're trying to use the cookie encryption feature"
      "on line %zu without having set the `.cookie_env_var` option in"
      "`sp.global`: please set it first.", sp_line_no);
    return -1;
  } else if (0 == (SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key)) {
    sp_log_err("config", "You're trying to use the cookie encryption feature"
      "on line %zu without having set the `.encryption_key` option in"
      "`sp.global`: please set it first.", sp_line_no);
    return -1;
  } else if (0 == strlen(name)) {
    sp_log_err("config", "You must specify a cookie name to encrypt on line "
      "%zu.", sp_line_no);
    return -1;
  }

  zend_hash_str_add_empty_element(
      SNUFFLEUPAGUS_G(config).config_cookie_encryption->names, name,
      strlen(name));

  return SUCCESS;
}

int parse_disabled_functions(char *line) {
  int ret = 0;
  bool enable = true, disable = false;
  char *pos = NULL;
  sp_disabled_function *df = pecalloc(sizeof(*df), 1, 1);
  df->pos = -1;

  sp_config_functions sp_config_funcs_disabled_functions[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_str, SP_TOKEN_ALIAS, &(df->alias)},
      {parse_empty, SP_TOKEN_SIMULATION, &(df->simulation)},
      {parse_str, SP_TOKEN_FILENAME, &(df->filename)},
      {parse_regexp, SP_TOKEN_FILENAME_REGEXP, &(df->r_filename)},
      {parse_str, SP_TOKEN_FUNCTION, &(df->function)},
      {parse_regexp, SP_TOKEN_FUNCTION_REGEXP, &(df->r_function)},
      {parse_str, SP_TOKEN_DUMP, &(df->dump)},
      {parse_empty, SP_TOKEN_ALLOW, &(df->allow)},
      {parse_empty, SP_TOKEN_DROP, &(df->drop)},
      {parse_str, SP_TOKEN_HASH, &(df->hash)},
      {parse_str, SP_TOKEN_PARAM, &(df->param)},
      {parse_regexp, SP_TOKEN_VALUE_REGEXP, &(df->value_r)},
      {parse_str, SP_TOKEN_VALUE, &(df->value)},
      {parse_regexp, SP_TOKEN_PARAM_REGEXP, &(df->r_param)},
      {parse_php_type, SP_TOKEN_PARAM_TYPE, &(df->param_type)},
      {parse_str, SP_TOKEN_RET, &(df->ret)},
      {parse_cidr, SP_TOKEN_CIDR, &(df->cidr)},
      {parse_regexp, SP_TOKEN_RET_REGEXP, &(df->r_ret)},
      {parse_php_type, SP_TOKEN_RET_TYPE, &(df->ret_type)},
      {parse_str, SP_TOKEN_LOCAL_VAR, &(df->var)},
      {parse_str, SP_TOKEN_VALUE_ARG_POS, &(pos)},
      {0}};

  ret = parse_keywords(sp_config_funcs_disabled_functions, line);

  if (0 != ret) {
    return ret;
  }

  if (true == disable){
    df->enable = false;
  } else {
    df->enable = true;
  }

  if (df->value && df->value_r) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "'.value' and '.regexp' are mutually exclusives on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (df->r_function && df->function) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s': "
               "'.r_function' and '.function' are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (df->r_filename && df->filename) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "'.r_filename' and '.filename' are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (df->r_param && df->param) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "'.r_param' and '.param' are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (df->r_ret && df->ret) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "'.r_ret' and '.ret' are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if ((df->r_ret || df->ret) && (df->r_param || df->param)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "`ret` and `param` are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (!(df->r_function || df->function)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               " must take a function name on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (!(df->allow ^ df->drop)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s': The "
               "rule must either be a `drop` or and `allow` one on line %zu.",
               line, sp_line_no);
    return -1;
  }

  if (pos) {
    errno = 0;
    char *endptr;
    df->pos = strtol(pos, &endptr, 10) > 128 ? 128 : strtol(pos, NULL, 10);
    if (errno != 0 || endptr == pos) {
      sp_log_err("config",
		 "Failed to parse arg '%s' of `pos` on line %zu.",
		 pos, sp_line_no);
      return -1;
    }
  }

  if (df->function) {
    df->functions_list = parse_functions_list(df->function);
  }

  if (df->param && strchr(df->param, '[')) {  // assume that this is an array
    df->param_array_keys = sp_new_list();
    if (0 != array_to_list(&df->param, &df->param_array_keys)) {
      pefree(df->param_array_keys, 1);
      return -1;
    }
    df->param_is_array = 1;
  }

  if (df->var && strchr(df->var, '[')) {  // assume that this is an array
    df->var_array_keys = sp_new_list();
    if (0 != array_to_list(&df->var, &df->var_array_keys)) {
      pefree(df->var_array_keys, 1);
      return -1;
    }
    df->var_is_array = 1;
  }

  switch (get_construct_type(df)) {
    case ZEND_INCLUDE_OR_EVAL:
      sp_list_insert(SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_include, df);
      return ret;
    case ZEND_ECHO:
    default:
      break;
  }

  if (df->ret || df->r_ret || df->ret_type) {
    sp_list_insert(
        SNUFFLEUPAGUS_G(config).config_disabled_functions_ret->disabled_functions,
        df);
  } else {
    sp_list_insert(
        SNUFFLEUPAGUS_G(config).config_disabled_functions->disabled_functions,
        df);
  }
  return ret;
}

int parse_upload_validation(char *line) {
  bool disable = false, enable = false;
  sp_config_functions sp_config_funcs_upload_validation[] = {
      {parse_str, SP_TOKEN_UPLOAD_SCRIPT,
       &(SNUFFLEUPAGUS_G(config).config_upload_validation->script)},
      {parse_empty, SP_TOKEN_SIMULATION,
       &(SNUFFLEUPAGUS_G(config).config_upload_validation->simulation)},
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {0}};

  int ret = parse_keywords(sp_config_funcs_upload_validation, line);

  if (0 != ret) {
    return ret;
  }

  if (!(enable ^ disable)) {
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu.", sp_line_no);
    return -1;
  }
  SNUFFLEUPAGUS_G(config).config_upload_validation->enable = enable;

  char const *script = SNUFFLEUPAGUS_G(config).config_upload_validation->script;

  if (!script) {
    sp_log_err("config", "The `script` directive is mandatory in '%s' on line %zu.",
      line, sp_line_no);
    return -1;
  } else if (-1 == access(script, F_OK)) {
    sp_log_err("config", "The `script` (%s) doesn't exist on line %zu.", script, sp_line_no);
    return -1;
  } else if (-1 == access(script, X_OK)) {
    sp_log_err("config", "The `script` (%s) isn't executable on line %zu.", script, sp_line_no);
    return -1;
  }

  return ret;
}
