#if defined(__FreeBSD__)
#define _WITH_GETLINE
#endif

#include "php_snuffleupagus.h"


static zend_result sp_process_config_root(sp_parsed_keyword *parsed_rule) {
  sp_config_keyword sp_func[] = {
    {parse_unserialize,         SP_TOKEN_UNSERIALIZE_HMAC, &(SPCFG(unserialize))},
    {parse_unserialize_noclass, SP_TOKEN_UNSERIALIZE_NOCLASS, &(SPCFG(unserialize_noclass))},
    {parse_enable,              SP_TOKEN_HARDEN_RANDOM, &(SPCFG(random).enable)},
    {parse_log_media,           SP_TOKEN_LOG_MEDIA, &(SPCFG(log_media))},
    {parse_ulong,               SP_TOKEN_LOG_MAX_LEN, &(SPCFG(log_max_len))},
    {parse_disabled_functions,  SP_TOKEN_DISABLE_FUNC, NULL},
    {parse_readonly_exec,       SP_TOKEN_READONLY_EXEC, &(SPCFG(readonly_exec))},
    {parse_enable,              SP_TOKEN_GLOBAL_STRICT, &(SPCFG(global_strict).enable)},
    {parse_upload_validation,   SP_TOKEN_UPLOAD_VALIDATION, &(SPCFG(upload_validation))},
    {parse_cookie,              SP_TOKEN_COOKIE_ENCRYPTION, NULL},
    {parse_global,              SP_TOKEN_GLOBAL, NULL},
    {parse_enable,              SP_TOKEN_AUTO_COOKIE_SECURE, &(SPCFG(auto_cookie_secure).enable)},
    {parse_enable,              SP_TOKEN_XXE_PROTECTION, &(SPCFG(xxe_protection).enable)},
    {parse_eval_filter_conf,    SP_TOKEN_EVAL_BLACKLIST, &(SPCFG(eval).blacklist)},
    {parse_eval_filter_conf,    SP_TOKEN_EVAL_WHITELIST, &(SPCFG(eval).whitelist)},
    {parse_session,             SP_TOKEN_SESSION_ENCRYPTION, &(SPCFG(session))},
    {parse_enable,              SP_TOKEN_SLOPPY_COMPARISON, &(SPCFG(sloppy).enable)},
    {parse_wrapper_whitelist,   SP_TOKEN_ALLOW_WRAPPERS, &(SPCFG(wrapper))},
    {parse_ini_protection,      SP_TOKEN_INI_PROTECTION, &(SPCFG(ini))},
    {parse_ini_entry,           SP_TOKEN_INI, NULL},
    {NULL, NULL, NULL}};
  return sp_process_rule(parsed_rule, sp_func);
}

zend_result sp_parse_config(const char *const filename) {
  FILE *fd = fopen(filename, "rb");
  if (fd == NULL) {
    sp_log_err("config", "Could not open configuration file %s : %s", filename, strerror(errno));
    return FAILURE;
  }

  size_t step = 8192;
  size_t max_len = step, len = 0;
  zend_string *data = zend_string_alloc(max_len, 0);
  char *ptr = ZSTR_VAL(data);

  size_t bytes;
  while ((bytes = fread(ptr, 1, max_len - len, fd))) {
    len += bytes;
    if (max_len - len <= 0) {
      max_len += step;
      data = zend_string_extend(data, max_len, 0);
      ptr = ZSTR_VAL(data) + len;
    } else {
      ptr += bytes;
    }
  }
  fclose(fd);

  data = zend_string_truncate(data, len, 0);
  ZSTR_VAL(data)[len] = 0;

  int ret = sp_config_scan(ZSTR_VAL(data), sp_process_config_root);

  zend_string_release_ex(data, 0);

  return ret;
}


zend_result sp_process_rule(sp_parsed_keyword *parsed_rule, const sp_config_keyword *const config_keywords) {
  for (sp_parsed_keyword *kw = parsed_rule; kw->kw; kw++) {
    bool found_kw = false;
    for (const sp_config_keyword *ckw = config_keywords; ckw->func; ckw++) {
      if (kw->kwlen == strlen(ckw->token) && !strncmp(kw->kw, ckw->token, kw->kwlen)) {
        if (ckw->func) {
          int ret = ckw->func(ckw->token, kw, ckw->retval);
          switch (ret) {
            case SP_PARSER_SUCCESS:
              break;
            case SP_PARSER_ERROR:
              return FAILURE;
            case SP_PARSER_STOP:
              return SUCCESS;
          }
        }
        found_kw = true;
        break;
      }
    }

    if (!found_kw) {
      zend_string *kwname = zend_string_init(kw->kw, kw->kwlen, 0);
      sp_log_err("config", "Unexpected keyword '%s' on line %d", ZSTR_VAL(kwname), kw->lineno);
      zend_string_release_ex(kwname, 0);
      return FAILURE;
    }
  }
  return SUCCESS;
}

#define CHECK_DUPLICATE_KEYWORD(retval) \
  if (*(void**)(retval)) { \
    sp_log_err("config", "duplicate keyword '%s' on line %zu", token, kw->lineno); \
    return SP_PARSER_ERROR; }


SP_PARSEKW_FN(parse_empty) {
  if (kw->arglen) {
    sp_log_err("config", "Unexpected argument for keyword '%s' - it should be '%s()' on line %zu", token, token, kw->lineno);
    return SP_PARSER_ERROR;
  }
  if (kw->argtype != SP_ARGTYPE_EMPTY) {
    sp_log_err("config", "Missing paranthesis for keyword '%s' - it should be '%s()' on line %zu", token, token, kw->lineno);
    return SP_PARSER_ERROR;
  }
  *(bool *)retval = true;
  return SP_PARSER_SUCCESS;
}

SP_PARSEKW_FN(parse_list) {
  CHECK_DUPLICATE_KEYWORD(retval);

  sp_list_node **list = retval;

  SP_PARSE_ARG(value);

  char* tmp = ZSTR_VAL(value);
  while (1) {
    const char* const tok = strsep(&tmp, ",");
    if (tok == NULL) {
      break;
    }
    *list = sp_list_insert(*list, zend_string_init(tok, strlen(tok), 1));
  }
  zend_string_release(value);

  return SP_PARSER_SUCCESS;
}

SP_PARSEKW_FN(parse_php_type) {
  SP_PARSE_ARG(value);

  if (zend_string_equals_literal_ci(value, "undef")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_UNDEF;
  } else if (zend_string_equals_literal_ci(value, "null")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_NULL;
  } else if (zend_string_equals_literal_ci(value, "true")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_TRUE;
  } else if (zend_string_equals_literal_ci(value, "false")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_FALSE;
  } else if (zend_string_equals_literal_ci(value, "long")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_LONG;
  } else if (zend_string_equals_literal_ci(value, "double")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_DOUBLE;
  } else if (zend_string_equals_literal_ci(value, "string")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_STRING;
  } else if (zend_string_equals_literal_ci(value, "array")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_ARRAY;
  } else if (zend_string_equals_literal_ci(value, "object")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_OBJECT;
  } else if (zend_string_equals_literal_ci(value, "resource")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_RESOURCE;
  } else if (zend_string_equals_literal_ci(value, "reference")) {
    *(sp_php_type *)retval = SP_PHP_TYPE_REFERENCE;
  } else {
    zend_string_release(value);
    sp_log_err("error", ".%s() is expecting a valid php type ('false', 'true',"
                " 'array'. 'object', 'long', 'double', 'null', 'resource', "
                "'reference', 'undef') on line %zu", token, kw->lineno);
    return SP_PARSER_ERROR;
  }
  zend_string_release(value);
  return SP_PARSER_SUCCESS;
}


SP_PARSEKW_FN(parse_str) {
  CHECK_DUPLICATE_KEYWORD(retval);
  SP_PARSE_ARG(value);

  *(zend_string **)retval = value;

  return SP_PARSER_SUCCESS;
}

SP_PARSEKW_FN(parse_int) {
  int ret = SP_PARSER_SUCCESS;
  SP_PARSE_ARG(value);

  char *endptr;
  errno = 0;
  *(int*)retval = (int)strtoimax(ZSTR_VAL(value), &endptr, 10);
  if (errno != 0 || !endptr || endptr == ZSTR_VAL(value)) {
    sp_log_err("config", "Failed to parse arg '%s' of `%s` on line %zu", ZSTR_VAL(value), token, kw->lineno);
    ret = SP_PARSER_ERROR;
  }
  zend_string_release(value);
  return ret;
}

SP_PARSEKW_FN(parse_ulong) {
  int ret = SP_PARSER_SUCCESS;
  SP_PARSE_ARG(value);

  char *endptr;
  errno = 0;
  *(u_long*)retval = (u_long)strtoul(ZSTR_VAL(value), &endptr, 10);
  if (errno != 0 || !endptr || endptr == ZSTR_VAL(value)) {
    sp_log_err("config", "Failed to parse arg '%s' of `%s` on line %zu", ZSTR_VAL(value), token, kw->lineno);
    ret = SP_PARSER_ERROR;
  }
  zend_string_release(value);
  return ret;
}

SP_PARSEKW_FN(parse_cidr) {
  CHECK_DUPLICATE_KEYWORD(retval);
  SP_PARSE_ARG(value);

  sp_cidr *cidr = pecalloc(sizeof(sp_cidr), 1, 1);

  if (0 != get_ip_and_cidr(ZSTR_VAL(value), cidr)) {
    pefree(cidr, 1);
    cidr = NULL;
  }

  *(sp_cidr **)retval = cidr;
  return cidr ? SP_PARSER_SUCCESS : SP_PARSER_ERROR;
}

SP_PARSEKW_FN(parse_regexp) {
  CHECK_DUPLICATE_KEYWORD(retval);
  SP_PARSE_ARG(value);

  sp_regexp *compiled_re = sp_regexp_compile(value);
  if (!compiled_re) {
    sp_log_err("config", "Invalid regexp '%s' for '.%s()' on line %zu", ZSTR_VAL(value), token, kw->lineno);
    zend_string_release_ex(value, 1);
    return SP_PARSER_ERROR;
  }

  *(sp_regexp **)retval = compiled_re;

  return SP_PARSER_SUCCESS;
}

void sp_free_disabled_function(void *data) {
  sp_disabled_function *df = data;

  sp_free_zstr(df->textual_representation);

  sp_free_zstr(df->filename);
  sp_regexp_free(df->r_filename);

  sp_free_zstr(df->function);
  sp_regexp_free(df->r_function);
  sp_list_free(df->functions_list, free);

  sp_free_zstr(df->hash);

  sp_tree_free(df->param);
  sp_regexp_free(df->r_param);

  sp_regexp_free(df->r_ret);
  sp_free_zstr(df->ret);

  sp_regexp_free(df->r_value);
  sp_free_zstr(df->value);

  sp_regexp_free(df->r_key);
  sp_free_zstr(df->key);

  sp_free_zstr(df->dump);
  sp_free_zstr(df->alias);

  // sp_list_free(df->param_array_keys);
  // sp_list_free(df->var_array_keys);

  sp_tree_free(df->var);

  pefree(df->cidr, 1);
}

void sp_free_cookie(void *data) {
  sp_cookie *c = data;
  if (c->name)
    zend_string_release_ex(c->name, 1);
  sp_regexp_free(c->name_r);
}

void sp_free_zstr(void *data) {
  if (data) {
    zend_string_release_ex((zend_string*)data, 1);
  }
}

void sp_free_ini_entry(void *data) {
  sp_ini_entry *entry = data;

  sp_free_zstr(entry->key);
  sp_free_zstr(entry->min);
  sp_free_zstr(entry->max);
  sp_regexp_free(entry->regexp);
  sp_free_zstr(entry->msg);
  sp_free_zstr(entry->set);
}
