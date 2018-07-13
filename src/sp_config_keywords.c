#include "php_snuffleupagus.h"
#include "zend_types.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

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

int parse_session(char *line) {
  sp_config_session *session = pecalloc(sizeof(sp_config_session), 1, 0);

  sp_config_functions sp_config_funcs_session_encryption[] = {
      {parse_empty, SP_TOKEN_ENCRYPT, &(session->encrypt)},
      {parse_empty, SP_TOKEN_SIMULATION, &(session->simulation)},
      {0}};
  int ret = parse_keywords(sp_config_funcs_session_encryption, line);
  if (0 != ret) {
    return ret;
  }
  if (session->encrypt) {
    if (0 == (SNUFFLEUPAGUS_G(config).config_snuffleupagus->cookies_env_var)) {
      sp_log_err(
          "config",
          "You're trying to use the session cookie encryption feature"
          "on line %zu without having set the `.cookie_env_var` option in"
          "`sp.global`: please set it first.",
          sp_line_no);
      pefree(session, 0);
      return -1;
    } else if (0 ==
               (SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key)) {
      sp_log_err(
          "config",
          "You're trying to use the session cookie encryption feature"
          "on line %zu without having set the `.encryption_key` option in"
          "`sp.global`: please set it first.",
          sp_line_no);
      pefree(session, 0);
      return -1;
    }
  }

  SNUFFLEUPAGUS_G(config).config_session->encrypt = session->encrypt;
  SNUFFLEUPAGUS_G(config).config_session->simulation = session->simulation;
  pefree(session, 0);
  return ret;
}

int parse_random(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_random->enable),
                      NULL);
}

int parse_sloppy_comparison(char *line) {
  return parse_enable(line, &(SNUFFLEUPAGUS_G(config).config_sloppy->enable),
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
  bool enable = false, disable = false;
  sp_config_unserialize *unserialize =
      SNUFFLEUPAGUS_G(config).config_unserialize;

  sp_config_functions sp_config_funcs[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_empty, SP_TOKEN_SIMULATION, &(unserialize->simulation)},
      {parse_str, SP_TOKEN_DUMP, &(unserialize->dump)},
      {0}};

  unserialize->textual_representation = zend_string_init(line, strlen(line), 1);

  int ret = parse_keywords(sp_config_funcs, line);
  if (0 != ret) {
    return ret;
  }

  if (!(enable ^ disable)) {
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu.",
               sp_line_no);
    return -1;
  }

  SNUFFLEUPAGUS_G(config).config_unserialize->enable = enable;

  return ret;
}

int parse_readonly_exec(char *line) {
  bool enable = false, disable = false;
  sp_config_readonly_exec *readonly_exec =
      SNUFFLEUPAGUS_G(config).config_readonly_exec;

  sp_config_functions sp_config_funcs[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_empty, SP_TOKEN_SIMULATION, &(readonly_exec->simulation)},
      {parse_str, SP_TOKEN_DUMP, &(readonly_exec->dump)},
      {0}};

  readonly_exec->textual_representation =
      zend_string_init(line, strlen(line), 1);
  int ret = parse_keywords(sp_config_funcs, line);

  if (0 != ret) {
    return ret;
  }

  if (!(enable ^ disable)) {
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu.",
               sp_line_no);
    return -1;
  }

  SNUFFLEUPAGUS_G(config).config_readonly_exec->enable = enable;

  return ret;
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

static int parse_eval_filter_conf(char *line, sp_list_node **list) {
  char *token, *tmp;
  zend_string *rest = NULL;
  sp_config_eval *eval = SNUFFLEUPAGUS_G(config).config_eval;

  sp_config_functions sp_config_funcs[] = {
      {parse_str, SP_TOKEN_EVAL_LIST, &rest},
      {parse_empty, SP_TOKEN_SIMULATION,
       &(SNUFFLEUPAGUS_G(config).config_eval->simulation)},
      {parse_str, SP_TOKEN_DUMP, &(SNUFFLEUPAGUS_G(config).config_eval->dump)},
      {0}};

  eval->textual_representation = zend_string_init(line, strlen(line), 1);

  int ret = parse_keywords(sp_config_funcs, line);
  if (0 != ret) {
    return ret;
  }

  tmp = ZSTR_VAL(rest);
  while ((token = strtok_r(tmp, ",", &tmp))) {
    *list = sp_list_insert(*list, zend_string_init(token, strlen(token), 1));
  }
  if (rest != NULL) {
    pefree(rest, 1);
  }
  return SUCCESS;
}

int parse_eval_blacklist(char *line) {
  return parse_eval_filter_conf(
      line, &SNUFFLEUPAGUS_G(config).config_eval->blacklist);
}

int parse_eval_whitelist(char *line) {
  return parse_eval_filter_conf(
      line, &SNUFFLEUPAGUS_G(config).config_eval->whitelist);
}

int parse_cookie(char *line) {
  int ret = 0;
  zend_string *samesite = NULL;
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
  if ((!cookie->name || 0 == ZSTR_LEN(cookie->name)) && !cookie->name_r) {
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
    if (zend_string_equals_literal_ci(samesite, SP_TOKEN_SAMESITE_LAX)) {
      cookie->samesite = lax;
    } else if (zend_string_equals_literal_ci(samesite,
                                             SP_TOKEN_SAMESITE_STRICT)) {
      cookie->samesite = strict;
    } else {
      sp_log_err(
          "config",
          "%s is an invalid value to samesite (expected %s or %s) on line "
          "%zu.",
          ZSTR_VAL(samesite), SP_TOKEN_SAMESITE_LAX, SP_TOKEN_SAMESITE_STRICT,
          sp_line_no);
      return -1;
    }
  }
  SNUFFLEUPAGUS_G(config).config_cookie->cookies =
      sp_list_insert(SNUFFLEUPAGUS_G(config).config_cookie->cookies, cookie);
  return SUCCESS;
}

int add_df_to_hashtable(HashTable *ht, sp_disabled_function *df) {
  zval *list = zend_hash_find(ht, df->function);

  if (NULL == list) {
    zend_hash_add_ptr(ht, df->function, sp_list_insert(NULL, df));
  } else {
    Z_PTR_P(list) = sp_list_insert(Z_PTR_P(list), df);
  }
  return SUCCESS;
}

int parse_disabled_functions(char *line) {
  int ret = 0;
  bool enable = true, disable = false, allow = false, drop = false;
  zend_string *pos = NULL, *var = NULL, *param = NULL;
  zend_string *line_number = NULL;
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
      {parse_regexp, SP_TOKEN_VALUE_REGEXP, &(df->r_value)},
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

  MUTUALLY_EXCLUSIVE(df->value, df->r_value, "value", "regexp");
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
  } else if ((df->r_key || df->key) && (df->r_value || df->value)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "`key` and `value` are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if ((df->r_ret || df->ret || df->ret_type) && (df->r_param || param)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "`ret` and `param` are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if ((df->r_ret || df->ret || df->ret_type) && (var)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "`ret` and `var` are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if ((df->r_ret || df->ret || df->ret_type) &&
             (df->value || df->r_value)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               "`ret` and `value` are mutually exclusive on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (!(df->r_function || df->function)) {
    sp_log_err("config",
               "Invalid configuration line: 'sp.disabled_functions%s':"
               " must take a function name on line %zu.",
               line, sp_line_no);
    return -1;
  } else if (df->filename && *ZSTR_VAL(df->filename) != '/') {
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
    df->pos = (int)strtol(ZSTR_VAL(pos), &endptr, 10);
    if (errno != 0 || endptr == ZSTR_VAL(pos)) {
      sp_log_err("config", "Failed to parse arg '%s' of `pos` on line %zu.",
                 ZSTR_VAL(pos), sp_line_no);
      return -1;
    }
  }

  if (line_number) {
    errno = 0;
    char *endptr;
    df->line = (unsigned int)strtoul(ZSTR_VAL(line_number), &endptr, 10);
    if (errno != 0 || endptr == ZSTR_VAL(line_number)) {
      sp_log_err("config", "Failed to parse arg '%s' of `line` on line %zu.",
                 ZSTR_VAL(line_number), sp_line_no);
      return -1;
    }
  }
  df->allow = allow;
  df->textual_representation = zend_string_init(line, strlen(line), 1);

  if (df->function) {
    df->functions_list = parse_functions_list(ZSTR_VAL(df->function));
  }

  if (param) {
    if (ZSTR_LEN(param) > 0 && ZSTR_VAL(param)[0] != '$') {
      /* This is an ugly hack. We're prefixing with a `$` because otherwise
       * the parser treats this as a constant.
       * FIXME: Remove this, and improve our (weird) parser. */
      char *new = pecalloc(ZSTR_LEN(param) + 2, 1, 1);
      new[0] = '$';
      memcpy(new + 1, ZSTR_VAL(param), ZSTR_LEN(param));
      df->param = sp_parse_var(new);
      free(new);
    } else {
      df->param = sp_parse_var(ZSTR_VAL(param));
    }
    if (!df->param) {
      sp_log_err("config", "Invalid value '%s' for `param` on line %zu.",
                 ZSTR_VAL(param), sp_line_no);
      return -1;
    }
  }

  if (var) {
    if (ZSTR_LEN(var)) {
      df->var = sp_parse_var(ZSTR_VAL(var));
      if (!df->var) {
        sp_log_err("config", "Invalid value '%s' for `var` on line %zu.",
                   ZSTR_VAL(var), sp_line_no);
        return -1;
      }
    } else {
      sp_log_err("config", "Empty value in `var` on line %zu.", sp_line_no);
      return -1;
    }
  }

  if (true == disable) {
    return ret;
  }

  if (df->function && zend_string_equals_literal(df->function, "print")) {
    zend_string_release(df->function);
    df->function = zend_string_init("echo", strlen("echo"), 1);
  }

  if (df->function && !df->functions_list) {
    if (df->ret || df->r_ret || df->ret_type) {
      add_df_to_hashtable(SNUFFLEUPAGUS_G(config).config_disabled_functions_ret,
                          df);
    } else {
      add_df_to_hashtable(SNUFFLEUPAGUS_G(config).config_disabled_functions,
                          df);
    }
  } else {
    if (df->ret || df->r_ret || df->ret_type) {
      SNUFFLEUPAGUS_G(config)
          .config_disabled_functions_reg_ret->disabled_functions =
          sp_list_insert(
              SNUFFLEUPAGUS_G(config)
                  .config_disabled_functions_reg_ret->disabled_functions,
              df);
    } else {
      SNUFFLEUPAGUS_G(config)
          .config_disabled_functions_reg->disabled_functions =
          sp_list_insert(SNUFFLEUPAGUS_G(config)
                             .config_disabled_functions_reg->disabled_functions,
                         df);
    }
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

  zend_string const *script =
      SNUFFLEUPAGUS_G(config).config_upload_validation->script;

  if (!script) {
    sp_log_err("config",
               "The `script` directive is mandatory in '%s' on line %zu.", line,
               sp_line_no);
    return -1;
  } else if (-1 == access(ZSTR_VAL(script), F_OK)) {
    sp_log_err("config", "The `script` (%s) doesn't exist on line %zu.",
               ZSTR_VAL(script), sp_line_no);
    return -1;
  } else if (-1 == access(ZSTR_VAL(script), X_OK)) {
    sp_log_err("config", "The `script` (%s) isn't executable on line %zu.",
               ZSTR_VAL(script), sp_line_no);
    return -1;
  }

  return ret;
}
