#include "php_snuffleupagus.h"

#define SP_SET_ENABLE_DISABLE(enable, disable, varname) \
  if (enable && disable) { \
    sp_log_err("config", "A rule can't be enabled and disabled on line %zu", parsed_rule->lineno); \
    return SP_PARSER_ERROR; \
  } \
  if (enable || disable) { \
    (varname) = (enable || !disable); \
  }

#define SP_PROCESS_CONFIG_KEYWORDS(CMD) if (sp_process_rule(&(parsed_rule[1]), config_keywords) != SUCCESS) { CMD; }
#define SP_PROCESS_CONFIG_KEYWORDS_ERR() SP_PROCESS_CONFIG_KEYWORDS(return SP_PARSER_ERROR)

SP_PARSE_FN(parse_enable) {
  bool enable = false, disable = false;
  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  SP_SET_ENABLE_DISABLE(enable, disable, *(bool*)retval);

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_session) {
  sp_config_session *cfg = retval;

  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENCRYPT, &(cfg->encrypt)},
      {parse_empty, SP_TOKEN_SIMULATION, &(cfg->simulation)},
      {parse_empty, SP_TOKEN_SIM, &(cfg->simulation)},
      {parse_ulong, SP_TOKEN_SID_MIN_LENGTH, &(cfg->sid_min_length)},
      {parse_ulong, SP_TOKEN_SID_MAX_LENGTH, &(cfg->sid_max_length)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  if (cfg->encrypt) {
    if (!SPCFG(cookies_env_var)) {
      sp_log_err("config", "You're trying to use the session cookie encryption feature "
          "on line %zu without having set the `.cookie_env_var` option in "
          "`sp.global`: please set it first", parsed_rule->lineno);
      return SP_PARSER_ERROR;
    } else if (!SPCFG(encryption_key)) {
      sp_log_err("config", "You're trying to use the session cookie encryption feature "
                 "on line %zu without having set the `.secret_key` option in "
                 "`sp.global`: please set it first", parsed_rule->lineno);
      return SP_PARSER_ERROR;
    }
  }

  return SP_PARSER_STOP;
}

SP_PARSEKW_FN(parse_log_media) {
  SP_PARSE_ARG(value);
  
  if (!strcmp(ZSTR_VAL(value), "php")) {
    *(char*)retval = SP_ZEND;
    zend_string_release_ex(value, 1);
    return SP_PARSER_SUCCESS;
  } else if (!strcmp(ZSTR_VAL(value), "syslog")) {
    *(char*)retval = SP_SYSLOG;
    zend_string_release_ex(value, 1);
    return SP_PARSER_SUCCESS;
  }

  sp_log_err("config", "." SP_TOKEN_LOG_MEDIA "() only supports 'syslog' or 'php' on line %zu", kw->lineno);

  return SP_PARSER_ERROR;
}

SP_PARSE_FN(parse_unserialize_noclass) {
  bool enable = false, disable = false;
  sp_config_unserialize_noclass *cfg = (sp_config_unserialize_noclass*)retval;

  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  SP_SET_ENABLE_DISABLE(enable, disable, cfg->enable);

  cfg->textual_representation = sp_get_textual_representation(parsed_rule);

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_unserialize) {
  bool enable = false, disable = false;
  sp_config_unserialize *cfg = (sp_config_unserialize*)retval;

  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_empty, SP_TOKEN_SIMULATION, &(cfg->simulation)},
      {parse_empty, SP_TOKEN_SIM, &(cfg->simulation)},
      {parse_str, SP_TOKEN_DUMP, &(cfg->dump)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  SP_SET_ENABLE_DISABLE(enable, disable, cfg->enable);

  cfg->textual_representation = sp_get_textual_representation(parsed_rule);

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_readonly_exec) {
  bool enable = false, disable = false, xchecks = false, no_xchecks = false;
  sp_config_readonly_exec *cfg = (sp_config_readonly_exec*)retval;

  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_empty, SP_TOKEN_SIMULATION, &(cfg->simulation)},
      {parse_empty, SP_TOKEN_SIM, &(cfg->simulation)},
      {parse_str, SP_TOKEN_DUMP, &(cfg->dump)},
      {parse_empty, "extended_checks", &(xchecks)},
      {parse_empty, "xchecks", &(xchecks)},
      {parse_empty, "no_extended_checks", &(no_xchecks)},
      {parse_empty, "noxchecks", &(no_xchecks)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  cfg->textual_representation = sp_get_textual_representation(parsed_rule);

  SP_SET_ENABLE_DISABLE(enable, disable, cfg->enable);
  if (xchecks) { cfg->extended_checks = true; } else if (no_xchecks) { cfg->extended_checks = false; }

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_global) {
  sp_config_keyword config_keywords[] = {
      {parse_str, SP_TOKEN_ENCRYPTION_KEY, &(SPCFG(encryption_key))},
      {parse_str, SP_TOKEN_ENV_VAR, &(SPCFG(cookies_env_var))},
      {parse_log_media, SP_TOKEN_LOG_MEDIA, &(SPCFG(log_media))},
      {parse_ulong, SP_TOKEN_LOG_MAX_LEN, &(SPCFG(log_max_len))},
      {parse_ulong, SP_TOKEN_MAX_EXECUTION_DEPTH, &(SPCFG(max_execution_depth))},
      {parse_enable, SP_TOKEN_SERVER_ENCODE, &(SPCFG(server_encode))},
      {parse_enable, SP_TOKEN_SERVER_STRIP, &(SPCFG(server_strip))},
      {parse_enable, SP_TOKEN_SHOW_OLD_PHP_WARNING, &(SPCFG(show_old_php_warning))},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  if (SPCFG(encryption_key)) {
    if (ZSTR_LEN(SPCFG(encryption_key)) < 10) {
      sp_log_err("config", "The encryption key set on line %zu is too short. please use at least 10 bytes", parsed_rule->lineno);
      return SP_PARSER_ERROR;
    }
    if (zend_string_equals_literal(SPCFG(encryption_key), "YOU _DO_ NEED TO CHANGE THIS WITH SOME RANDOM CHARACTERS.") ||
        zend_string_equals_literal(SPCFG(encryption_key), "c6a0e02b3b818f7559d5f85303d8fe44")) {
      sp_log_err("config", "The encryption key set on line %zu is an unchanged dummy value. please use a unique secret.", parsed_rule->lineno);
      return SP_PARSER_ERROR;
    }
  }

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_eval_filter_conf) {
  sp_config_eval *cfg = &(SPCFG(eval));

  sp_config_keyword config_keywords[] = {
      {parse_list, SP_TOKEN_LIST, retval},
      {parse_empty, SP_TOKEN_SIMULATION, &(cfg->simulation)},
      {parse_empty, SP_TOKEN_SIM, &(cfg->simulation)},
      {parse_str, SP_TOKEN_DUMP, &(cfg->dump)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  cfg->textual_representation = sp_get_textual_representation(parsed_rule);

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_wrapper_whitelist) {
  sp_config_wrapper *cfg = (sp_config_wrapper*)retval;
  
  sp_config_keyword config_keywords[] = {
      {parse_list, SP_TOKEN_LIST, &cfg->whitelist},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  cfg->enabled = true;

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_cookie) {
  zend_string *samesite = NULL;
  sp_cookie *cookie = pecalloc(sizeof(sp_cookie), 1, 1);

  sp_config_keyword config_keywords[] = {
      {parse_str, SP_TOKEN_NAME, &(cookie->name)},
      {parse_regexp, SP_TOKEN_NAME_REGEXP, &(cookie->name_r)},
      {parse_str, SP_TOKEN_SAMESITE, &samesite},
      {parse_empty, SP_TOKEN_ENCRYPT, &cookie->encrypt},
      {parse_empty, SP_TOKEN_SIMULATION, &cookie->simulation},
      {parse_empty, SP_TOKEN_SIM, &cookie->simulation},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS(goto err);

  if (cookie->encrypt) {
    if (!SPCFG(cookies_env_var)) {
      sp_log_err("config", "You're trying to use the cookie encryption feature on line %zu "
                            "without having set the `." SP_TOKEN_ENV_VAR "` option in `sp.global`: please set it first", parsed_rule->lineno);
      goto err;
    } else if (!SPCFG(encryption_key)) {
      sp_log_err("config", "You're trying to use the cookie encryption feature "
          "on line %zu without having set the `." SP_TOKEN_ENCRYPTION_KEY "` option in "
          "`sp." SP_TOKEN_GLOBAL "`: please set it first", parsed_rule->lineno);
      goto err;
    }
  } else if (!samesite) {
    sp_log_err("config", "You must specify a at least one action to a cookie on line %zu", parsed_rule->lineno);
    goto err;
  }
  if ((!cookie->name || 0 == ZSTR_LEN(cookie->name)) && !cookie->name_r) {
    sp_log_err("config", "You must specify a cookie name/regexp on line %zu", parsed_rule->lineno);
    goto err;
  }
  if (cookie->name && cookie->name_r) {
    sp_log_err("config", "name and name_r are mutually exclusive on line %zu", parsed_rule->lineno);
    goto err;
  }
  if (samesite) {
    if (zend_string_equals_literal_ci(samesite, SP_TOKEN_SAMESITE_LAX)) {
      cookie->samesite = lax;
    } else if (zend_string_equals_literal_ci(samesite, SP_TOKEN_SAMESITE_STRICT)) {
      cookie->samesite = strict;
    } else {
      sp_log_err("config", "'%s' is an invalid value to samesite (expected " SP_TOKEN_SAMESITE_LAX " or " SP_TOKEN_SAMESITE_STRICT ") on line %zu",
        ZSTR_VAL(samesite), parsed_rule->lineno);
      goto err;
    }
    zend_string_release(samesite);
    samesite = NULL;
  }

  // find other cookie entry with identical name or name_r
  sp_cookie *entry = NULL;
  sp_list_node *pList = NULL;
  for (pList = SPCFG(cookie).cookies; pList; pList = pList->next) {
    entry = pList->data;
    if (!entry) { continue; }
    if ((entry->name && cookie->name && sp_zend_string_equals(entry->name, cookie->name)) ||
        (entry->name_r && cookie->name_r && sp_zend_string_equals(entry->name_r->pattern, cookie->name_r->pattern))) {
          break;
        }
  }
  if (pList && entry) {
    // override cookie settings if set
    if (cookie->samesite) { entry->samesite = cookie->samesite; }
    if (cookie->encrypt) { entry->encrypt = cookie->encrypt; }
    if (cookie->simulation) { entry->simulation = cookie->simulation; }
    sp_free_cookie(cookie);
    pefree(cookie, 1);
    cookie = NULL;
  } else {
    SPCFG(cookie).cookies = sp_list_insert(SPCFG(cookie).cookies, cookie);
  }

  return SP_PARSER_STOP;

err:
  if (samesite) {
    zend_string_release(samesite);
  }
  if (cookie) {
    sp_free_cookie(cookie);
    pefree(cookie, 1);
  }
  return SP_PARSER_ERROR;
}

static int add_df_to_hashtable(HashTable *ht, sp_disabled_function *df) {
  zval *list = zend_hash_find(ht, df->function);

  if (NULL == list) {
    zend_hash_add_ptr(ht, df->function, sp_list_insert(NULL, df));
  } else {
    Z_PTR_P(list) = sp_list_insert(Z_PTR_P(list), df);
  }
  return SUCCESS;
}

SP_PARSE_FN(parse_disabled_functions) {
  int ret = SP_PARSER_ERROR;
  bool enable = false, disable = false, allow = false, drop = false;
  zend_string *var = NULL, *param = NULL;
  sp_disabled_function *df = pecalloc(sizeof(*df), 1, 1);
  df->pos = -1;

  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_str, SP_TOKEN_ALIAS, &(df->alias)},
      {parse_empty, SP_TOKEN_SIMULATION, &(df->simulation)},
      {parse_empty, SP_TOKEN_SIM, &(df->simulation)},
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
      {parse_int, SP_TOKEN_VALUE_ARG_POS, &(df->pos)},
      {parse_ulong, SP_TOKEN_LINE_NUMBER, &(df->line)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS(goto out);

  SP_SET_ENABLE_DISABLE(enable, disable, enable);
  if (disable) {
    ret = SP_PARSER_STOP; goto out;
  }

#define MUTUALLY_EXCLUSIVE(X, Y, STR1, STR2)                             \
  if (X && Y) {                                                          \
    sp_log_err("config", "Invalid configuration line for 'sp.disabled_functions': '.%s' and '.%s' are mutually exclusive on line %zu", STR1, STR2, parsed_rule->lineno); \
    goto out;                                                  \
  }

  MUTUALLY_EXCLUSIVE(df->value, df->r_value, "value", "value_r");
  MUTUALLY_EXCLUSIVE(df->r_function, df->function, "function", "function_r");
  MUTUALLY_EXCLUSIVE(df->filename, df->r_filename, "filename", "filename_r");
  MUTUALLY_EXCLUSIVE(df->ret, df->r_ret, "ret", "ret_r");
  MUTUALLY_EXCLUSIVE(df->key, df->r_key, "key", "key_r");
  MUTUALLY_EXCLUSIVE((df->pos >= 0), param, "pos", "param");
  MUTUALLY_EXCLUSIVE((df->pos >= 0), df->r_param, "pos", "param_r");
  MUTUALLY_EXCLUSIVE(param, df->r_param, "param", "param_r");
  MUTUALLY_EXCLUSIVE((df->r_key || df->key), (df->r_value || df->value), "key", "value");
  MUTUALLY_EXCLUSIVE((df->r_ret || df->ret || df->ret_type), (df->r_param || param), "ret", "param");
  MUTUALLY_EXCLUSIVE((df->r_ret || df->ret || df->ret_type), (var), "ret", "var");
  MUTUALLY_EXCLUSIVE((df->r_ret || df->ret || df->ret_type), (df->value || df->r_value), "ret", "value");

#undef MUTUALLY_EXCLUSIVE

  if (!(df->r_function || df->function)) {
    sp_log_err("config", "Invalid configuration line: 'sp.disabled_functions': must take a function name on line %zu", parsed_rule->lineno);
    goto out;
  }
  if (df->filename && (*ZSTR_VAL(df->filename) != '/') &&
             (0 != strncmp(ZSTR_VAL(df->filename), "phar://", sizeof("phar://")))) {
    sp_log_err("config", "Invalid configuration line: 'sp.disabled_functions': '.filename' must be an absolute path or a phar archive on line %zu", parsed_rule->lineno);
    goto out;
  }
  if (!(allow ^ drop)) {
    sp_log_err("config", "Invalid configuration line: 'sp.disabled_functions': The rule must either be a `drop` or `allow` one on line %zu", parsed_rule->lineno);
    goto out;
  }

  df->allow = allow;
  df->textual_representation = sp_get_textual_representation(parsed_rule);

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
      pefree(new, 1);
    } else {
      df->param = sp_parse_var(ZSTR_VAL(param));
    }
    if (!df->param) {
      sp_log_err("config", "Invalid value '%s' for `param` on line %zu", ZSTR_VAL(param), parsed_rule->lineno);
      goto out;
    }
  }
  if (var) {
    if (ZSTR_LEN(var)) {
      df->var = sp_parse_var(ZSTR_VAL(var));
      if (!df->var) {
        sp_log_err("config", "Invalid value '%s' for `var` on line %zu", ZSTR_VAL(var), parsed_rule->lineno);
        goto out;
      }
    } else {
      sp_log_err("config", "Empty value in `var` on line %zu", parsed_rule->lineno);
      goto out;
    }
  }

  if (df->function && zend_string_equals_literal(df->function, "print")) {
    zend_string_release(df->function);
    df->function = zend_string_init(ZEND_STRL("echo"), 1);
  }

  if (df->function && !df->functions_list) {
    if (df->ret || df->r_ret || df->ret_type) {
      add_df_to_hashtable(SPCFG(disabled_functions_ret), df);
    } else {
      add_df_to_hashtable(SPCFG(disabled_functions), df);
    }
  } else {
    if (df->ret || df->r_ret || df->ret_type) {
      SPCFG(disabled_functions_reg_ret).disabled_functions = sp_list_insert(SPCFG(disabled_functions_reg_ret).disabled_functions, df);
    } else {
      SPCFG(disabled_functions_reg).disabled_functions = sp_list_insert(SPCFG(disabled_functions_reg).disabled_functions, df);
    }
  }
  return SP_PARSER_STOP;

out:
  sp_free_disabled_function(df);
  pefree(df, 1);
  if (param) { zend_string_release(param); }
  if (var) { zend_string_release(var); }

  return ret;
}

SP_PARSE_FN(parse_upload_validation) {
  bool disable = false, enable = false;
  sp_config_upload_validation *cfg = (sp_config_upload_validation*)retval;

  sp_config_keyword config_keywords[] = {
      {parse_empty, SP_TOKEN_ENABLE, &(enable)},
      {parse_empty, SP_TOKEN_DISABLE, &(disable)},
      {parse_str, SP_TOKEN_UPLOAD_SCRIPT, &(cfg->script)},
      {parse_empty, SP_TOKEN_SIMULATION, &(cfg->simulation)},
      {parse_empty, SP_TOKEN_SIM, &(cfg->simulation)},
      {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();
  SP_SET_ENABLE_DISABLE(enable, disable, cfg->enable);

  if (!cfg->script) {
    sp_log_err("config", "The `script` directive is mandatory in '.%s' on line %zu", token, parsed_rule->lineno);
    return SP_PARSER_ERROR;
  } else if (-1 == access(ZSTR_VAL(cfg->script), F_OK)) {
    sp_log_err("config", "The `script` (%s) doesn't exist on line %zu", ZSTR_VAL(cfg->script), parsed_rule->lineno);
    return SP_PARSER_ERROR;
  }

  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_ini_protection) {
  bool disable = false, enable = false;
  bool rw = false, ro = false; // rw is ignored, but declaring .policy_rw is valid for readability
  sp_config_ini *cfg = (sp_config_ini*)retval;
  sp_config_keyword config_keywords[] = {
    {parse_empty, "enable", &(enable)},
    {parse_empty, "disable", &(disable)},
    {parse_empty, "simulation", &cfg->simulation},
    {parse_empty, "sim", &cfg->simulation},
    {parse_empty, "policy_readonly", &ro},
    {parse_empty, "policy_ro", &ro},
    {parse_empty, "policy_readwrite", &rw},
    {parse_empty, "policy_rw", &rw},
    {parse_empty, "policy_silent_ro", &cfg->policy_silent_ro},
    {parse_empty, "policy_silent_fail", &cfg->policy_silent_fail},
    {parse_empty, "policy_no_log", &cfg->policy_silent_fail},
    {parse_empty, "policy_drop", &cfg->policy_drop},
    {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS_ERR();

  SP_SET_ENABLE_DISABLE(enable, disable, cfg->enable);

  if (ro && rw) {
    sp_log_err("config", "rule cannot be both read-write and read-only on line %zu", parsed_rule->lineno);
    return SP_PARSER_ERROR;
  }
  cfg->policy_readonly = ro;

  if (cfg->policy_silent_fail && cfg->policy_drop) {
    sp_log_err("config", "policy cannot be drop and silent at the same time on line %zu", parsed_rule->lineno);
    return SP_PARSER_ERROR;
  }
  return SP_PARSER_STOP;
}

SP_PARSE_FN(parse_ini_entry) {
  sp_ini_entry *entry = pecalloc(sizeof(sp_ini_entry), 1, 1);
  bool rw = false, ro = false;

  sp_config_keyword config_keywords[] = {
    {parse_empty, "simulation", &entry->simulation},
    {parse_empty, "sim", &entry->simulation},
    {parse_str, "key", &entry->key},
    {parse_str, "msg", &entry->msg},
    {parse_str, "set", &entry->set},
    {parse_str, "min", &entry->min},
    {parse_str, "max", &entry->max},
    {parse_regexp, "regexp", &entry->regexp},
    {parse_empty, "readonly", &ro},
    {parse_empty, "ro", &ro},
    {parse_empty, "readwrite", &rw},
    {parse_empty, "rw", &rw},
    {parse_empty, "drop", &entry->drop},
    {parse_empty, "allow_null", &entry->allow_null},
    {0, 0, 0}};

  SP_PROCESS_CONFIG_KEYWORDS(goto err);

   if (!entry->key) {
    sp_log_err("config", "A .key() must be provided on line %zu", parsed_rule->lineno);
    goto err;
  }

  if (zend_hash_find_ptr(SPCFG(ini).entries, entry->key)) {
    sp_log_err("config", "duplicate INI key '%s' on line %zu", ZSTR_VAL(entry->key), parsed_rule->lineno);
    goto err;
  }

  if (ro && rw) {
    sp_log_err("config", "rule cannot be both read-write and read-only on line %zu", parsed_rule->lineno);
    goto err;
  }
  entry->access = ro - rw;

  zend_hash_add_ptr(SPCFG(ini).entries, entry->key, entry);
  return SP_PARSER_STOP;

err:
  if (entry) {
    sp_free_ini_entry(entry);
    pefree(entry, 1);
  }
  return SP_PARSER_ERROR;
}
