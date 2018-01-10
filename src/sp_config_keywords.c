#include "php_snuffleupagus.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static const struct {
  unsigned int type;
  char *keys[5];  // Update this value if necessary
} CONSTRUCTS_TYPES[] = {
    {.type = ZEND_INCLUDE_OR_EVAL,
     .keys = {"include", "include_once", "require", "require_once", NULL}},
    {.type = ZEND_ECHO, .keys = {"echo", NULL}},
    {.type = ZEND_NEW, .keys = {"new", NULL}},
    {.type = ZEND_EXIT, .keys = {"exit", NULL}},
    {.type = ZEND_STRLEN, .keys = {"strlen", NULL}},
    {.type = ZEND_EVAL_CODE, .keys = {"eval", NULL}},
    {.type = 0, .keys = {NULL}}};

static int get_construct_type(sp_disabled_function const *const df) {
  for (size_t i = 0; 0 != CONSTRUCTS_TYPES[i].type; i++) {
    for (size_t j = 0; NULL != CONSTRUCTS_TYPES[i].keys[j]; j++) {
      assert(df->function || df->r_function);
      if (df->function) {
        if (0 == strcmp(df->function, CONSTRUCTS_TYPES[i].keys[j])) {
          return CONSTRUCTS_TYPES[i].type;
        }
      } else {
        if (true ==
            is_regexp_matching(df->r_function, CONSTRUCTS_TYPES[i].keys[j])) {
          return CONSTRUCTS_TYPES[i].type;
        }
      }
    }
  }
  return -1;
}

static int parse_enable(char *line, bool *restrict retval,
                        bool *restrict simulation) {
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
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu.",
               sp_line_no);
    return -1;
  }

  *retval = enable;

  return ret;
}

int parse_random(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_random->enable),
                      NULL);
}

int parse_disable_xxe(char *line) {
  return parse_enable(
      line, &(SNUFFLEUPAGUS_G(config).config_disable_xxe->enable), NULL);
}

int parse_auto_cookie_secure(char *line) {
  return parse_enable(
      line, &(SNUFFLEUPAGUS_G(config).config_auto_cookie_secure->enable), NULL);
}

int parse_global_strict(char *line) {
  return parse_enable(
      line, &(SNUFFLEUPAGUS_G(config).config_global_strict->enable), NULL);
}

int parse_unserialize(char *line) {
  return parse_enable(
      line, &(SNUFFLEUPAGUS_G(config).config_unserialize->enable),
      &(SNUFFLEUPAGUS_G(config).config_unserialize->simulation));
}

int parse_readonly_exec(char *line) {
  return parse_enable(
      line, &(SNUFFLEUPAGUS_G(config).config_readonly_exec->enable),
      &(SNUFFLEUPAGUS_G(config).config_readonly_exec->simulation));
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

static int parse_eval_filter_conf(char *line, sp_list_node *list) {
  char *token;
  char *rest;
  sp_config_functions sp_config_funcs[] = {
      {parse_str, SP_TOKEN_EVAL_LIST, &rest},
      {parse_empty, SP_TOKEN_SIMULATION,
       &(SNUFFLEUPAGUS_G(config).config_eval->simulation)},
      {0}};
  int ret = parse_keywords(sp_config_funcs, line);
  if (0 != ret) {
    return ret;
  }

  while ((token = strtok_r(rest, ",", &rest))) {
    sp_list_insert(list, token);
  }
  return SUCCESS;
}

int parse_eval_blacklist(char *line) {
  return parse_eval_filter_conf(line,
                                SNUFFLEUPAGUS_G(config).config_eval->blacklist);
}

int parse_eval_whitelist(char *line) {
  return parse_eval_filter_conf(line,
                                SNUFFLEUPAGUS_G(config).config_eval->whitelist);
}

int parse_cookie(char *line) {
  int ret = 0;
  char *samesite = NULL;
  sp_cookie *cookie = pecalloc(sizeof(sp_cookie), 1, 1);

  sp_config_functions sp_config_funcs_cookie_encryption[] = {
      {parse_str, SP_TOKEN_NAME, &(cookie->name)},
      {parse_regexp, SP_TOKEN_NAME_REGEXP, &(cookie->name_r)},
      {parse_str, SP_TOKEN_SAMESITE, &samesite},
      {parse_empty, SP_TOKEN_ENCRYPT, &cookie->encrypt},
      {parse_empty, SP_TOKEN_SIMULATION, &cookie->simulation},
      {0}};

  ret = parse_keywords(sp_config_funcs_cookie_encryption, line);
  if (0 != ret) {
    return ret;
  }

  if (cookie->encrypt) {
    if (0 == (SNUFFLEUPAGUS_G(config).config_snuffleupagus->cookies_env_var)) {
      sp_log_err(
          "config",
          "You're trying to use the cookie encryption feature"
          "on line %zu without having set the `.cookie_env_var` option in"
          "`sp.global`: please set it first.",
          sp_line_no);
      return -1;
    } else if (0 ==
               (SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key)) {
      sp_log_err(
          "config",
          "You're trying to use the cookie encryption feature"
          "on line %zu without having set the `.encryption_key` option in"
          "`sp.global`: please set it first.",
          sp_line_no);
      return -1;
    }
  } else if (!samesite) {
    sp_log_err("config",
               "You must specify a at least one action to a cookie on line "
               "%zu.",
               sp_line_no);
    return -1;
  }
  if ((!cookie->name || '\0' == cookie->name[0]) && !cookie->name_r) {
    sp_log_err("config",
               "You must specify a cookie name/regexp on line "
               "%zu.",
               sp_line_no);
    return -1;
  }
  if (cookie->name && cookie->name_r) {
    sp_log_err("config",
               "name and name_r are mutually exclusive on line "
               "%zu.",
               sp_line_no);
    return -1;
  }
  if (samesite) {
    if (0 == strcasecmp(samesite, SP_TOKEN_SAMESITE_LAX)) {
      cookie->samesite = lax;
    } else if (0 == strcasecmp(samesite, SP_TOKEN_SAMESITE_STRICT)) {
      cookie->samesite = strict;
    } else {
      sp_log_err(
          "config",
          "%s is an invalid value to samesite (expected %s or %s) on line "
          "%zu.",
          samesite, SP_TOKEN_SAMESITE_LAX, SP_TOKEN_SAMESITE_STRICT,
          sp_line_no);
      return -1;
    }
  }
  sp_list_insert(SNUFFLEUPAGUS_G(config).config_cookie->cookies, cookie);
  return SUCCESS;
}

int parse_disabled_functions(char *line) {
  int ret = 0;
  bool enable = true, disable = false, allow = false, drop = false;
  char *pos = NULL, *var = NULL, *param = NULL;
  char *line_number = NULL;
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
      {parse_empty, SP_TOKEN_ALLOW, &(allow)},
      {parse_empty, SP_TOKEN_DROP, &(drop)},
      {parse_str, SP_TOKEN_HASH, &(df->hash)},
      {parse_str, SP_TOKEN_PARAM, &(param)},
      {parse_regexp, SP_TOKEN_VALUE_REGEXP, &(df->value_r)},
      {parse_str, SP_TOKEN_VALUE, &(df->value)},
      {parse_str, SP_TOKEN_KEY, &(df->key)},
      {parse_regexp, SP_TOKEN_KEY_REGEXP, &(df->r_key)},
      {parse_regexp, SP_TOKEN_PARAM_REGEXP, &(df->r_param)},
      {parse_php_type, SP_TOKEN_PARAM_TYPE, &(df->param_type)},
      {parse_str, SP_TOKEN_RET, &(df->ret)},
      {parse_cidr, SP_TOKEN_CIDR, &(df->cidr)},
      {parse_regexp, SP_TOKEN_RET_REGEXP, &(df->r_ret)},
      {parse_php_type, SP_TOKEN_RET_TYPE, &(df->ret_type)},
      {parse_str, SP_TOKEN_LOCAL_VAR, &(var)},
      {parse_str, SP_TOKEN_VALUE_ARG_POS, &(pos)},
      {parse_str, SP_TOKEN_LINE_NUMBER, &(line_number)},
      {0}};

  ret = parse_keywords(sp_config_funcs_disabled_functions, line);

  if (0 != ret) {
    return ret;
  }

#define MUTUALLY_EXCLUSIVE(X, Y, STR1, STR2)                             \
  if (X && Y) {                                                          \
    sp_log_err("config",                                                 \
               "Invalid configuration line: 'sp.disabled_functions%s': " \
               "'.%s' and '.%s' are mutually exclusive on line %zu.",    \
               line, STR1, STR2, sp_line_no);                            \
    return 1;                                                            \
  }

  MUTUALLY_EXCLUSIVE(df->value, df->value_r, "value", "regexp");
  MUTUALLY_EXCLUSIVE(df->r_function, df->function, "r_function", "function");
  MUTUALLY_EXCLUSIVE(df->filename, df->r_filename, "r_filename", "filename");
  MUTUALLY_EXCLUSIVE(df->ret, df->r_ret, "r_ret", "ret");
  MUTUALLY_EXCLUSIVE(df->key, df->r_key, "r_key", "key");
#undef MUTUALLY_EXCLUSIVE

  if (1 <
      ((df->r_param ? 1 : 0) + (param ? 1 : 0) + ((-1 != df->pos) ? 1 : 0))) {
    sp_log_err(
        "config",
        "Invalid configuration line: 'sp.disabled_functions%s':"
        "'.r_param', '.param' and '.pos' are mutually exclusive on line %zu.",
        line, sp_line_no);
    return -1;
  } else if ((df->r_key || df->key) && (df->value_r || df->value)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "`key` and `value` are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if ((df->r_ret || df->ret) && (df->r_param || param)) {
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
  } else if (df->filename && *df->filename != '/') {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "'.filename' must be an absolute path on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (!(allow ^ drop)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s': The "
               "rule must either be a `drop` or `allow` one on line %zu.",
               line, sp_line_no);
    return -1;
  }

  if (pos) {
    errno = 0;
    char *endptr;
    df->pos = (int)strtol(pos, &endptr, 10);
    if (errno != 0 || endptr == pos) {
      sp_log_err("config", "Failed to parse arg '%s' of `pos` on line %zu.",
                 pos, sp_line_no);
      return -1;
    }
  }

  if (line_number) {
    errno = 0;
    char *endptr;
    df->line = (unsigned int)strtoul(line_number, &endptr, 10);
    if (errno != 0 || endptr == line_number) {
      sp_log_err("config", "Failed to parse arg '%s' of `line` on line %zu.",
                 line_number, sp_line_no);
      return -1;
    }
  }
  df->allow = allow;
  df->textual_representation = estrdup(line);

  if (df->function) {
    df->functions_list = parse_functions_list(df->function);
  }

  if (param) {
    df->param = parse_var(param);
    if (!df->param) {
      sp_log_err("config", "Invalid value '%s' for `param` on line %zu.", param,
                 sp_line_no);
      return -1;
    }
  }

  if (var) {
    if (*var) {
      df->var = parse_var(var);
      if (!df->var) {
        sp_log_err("config", "Invalid value '%s' for `var` on line %zu.", var,
                   sp_line_no);
        return -1;
      }
    } else {
      sp_log_err("config", "Empty value in `var` on line %zu.", sp_line_no);
      return -1;
    }
  }

  switch (get_construct_type(df)) {
    case ZEND_INCLUDE_OR_EVAL:
      sp_list_insert(
          SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_include,
          df);
      return ret;
    case ZEND_EVAL_CODE:
      sp_list_insert(
          SNUFFLEUPAGUS_G(config).config_disabled_constructs->construct_eval,
          df);
      return ret;
    case ZEND_ECHO:
    default:
      break;
  }

  if (true == disable) {
    return ret;
  }

  if (df->ret || df->r_ret || df->ret_type) {
    sp_list_insert(SNUFFLEUPAGUS_G(config)
                       .config_disabled_functions_ret->disabled_functions,
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
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu.",
               sp_line_no);
    return -1;
  }
  SNUFFLEUPAGUS_G(config).config_upload_validation->enable = enable;

  char const *script = SNUFFLEUPAGUS_G(config).config_upload_validation->script;

  if (!script) {
    sp_log_err("config",
               "The `script` directive is mandatory in '%s' on line %zu.", line,
               sp_line_no);
    return -1;
  } else if (-1 == access(script, F_OK)) {
    sp_log_err("config", "The `script` (%s) doesn't exist on line %zu.", script,
               sp_line_no);
    return -1;
  } else if (-1 == access(script, X_OK)) {
    sp_log_err("config", "The `script` (%s) isn't executable on line %zu.",
               script, sp_line_no);
    return -1;
  }

  return ret;
}
