#include "php_snuffleupagus.h"

#ifndef ZEND_EXT_API
#define ZEND_EXT_API ZEND_DLEXPORT
#endif

static PHP_INI_MH(OnUpdateConfiguration);
static inline void sp_op_array_handler(zend_op_array *op);

#ifdef SP_DEBUG_STDERR
int sp_debug_stderr = STDERR_FILENO;
#endif

ZEND_EXTENSION();

// LCOV_EXCL_START
ZEND_DLEXPORT int sp_zend_startup(zend_extension *extension) {
  return zend_startup_module(&snuffleupagus_module_entry);
}
// LCOV_EXCL_END

static inline void sp_op_array_handler(zend_op_array *const op) {
  // We need a filename, and strict mode not already enabled on this op
  if (NULL == op->filename) {
    return;
  } else {
    if (SPCFG(global_strict).enable) {
      op->fn_flags |= ZEND_ACC_STRICT_TYPES;
    }
  }
}

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

PHP_INI_BEGIN()
PHP_INI_ENTRY("sp.configuration_file", "", PHP_INI_SYSTEM, OnUpdateConfiguration)
STD_PHP_INI_BOOLEAN("sp.allow_broken_configuration", "0", PHP_INI_SYSTEM, OnUpdateBool, allow_broken_configuration, zend_snuffleupagus_globals, snuffleupagus_globals)

PHP_INI_END()

ZEND_DLEXPORT zend_extension zend_extension_entry = {
    PHP_SNUFFLEUPAGUS_EXTNAME,
    PHP_SNUFFLEUPAGUS_VERSION,
    PHP_SNUFFLEUPAGUS_AUTHOR,
    PHP_SNUFFLEUPAGUS_URL,
    PHP_SNUFFLEUPAGUS_COPYRIGHT,
    sp_zend_startup,
    NULL,
    NULL,                /* activate_func_t */
    NULL,                /* deactivate_func_t */
    NULL,                /* message_handler_func_t */
    sp_op_array_handler, /* op_array_handler_func_t */
    NULL,                /* statement_handler_func_t */
    NULL,                /* fcall_begin_handler_func_t */
    NULL,                /* fcall_end_handler_func_t */
    NULL,                /* op_array_ctor_func_t */
    NULL,                /* op_array_dtor_func_t */
    STANDARD_ZEND_EXTENSION_PROPERTIES};

static void sp_load_other_modules() {
  // try to load other modules before initializing Snuffleupagus
  zend_module_entry *module;
  bool should_start = false;
  ZEND_HASH_FOREACH_PTR(&module_registry, module) {
    if (should_start) {
      sp_log_debug("attempting to start module '%s' early", module->name);
      if (zend_startup_module_ex(module) != SUCCESS) {
        // startup failed. let's try again later.
        module->module_started = 0;
      }
    }
    if (strcmp(module->name, PHP_SNUFFLEUPAGUS_EXTNAME) == 0) {
      should_start = true;
    }
  } ZEND_HASH_FOREACH_END();


}

static PHP_GINIT_FUNCTION(snuffleupagus) {
#ifdef SP_DEBUG_STDERR
  if (getenv("SP_NODEBUG")) {
    sp_debug_stderr = -1;
  } else {
    sp_debug_stderr = dup(STDERR_FILENO);
  }
#endif
  sp_log_debug("(GINIT)");
  sp_load_other_modules();
  snuffleupagus_globals->is_config_valid = SP_CONFIG_NONE;
  snuffleupagus_globals->in_eval = 0;
  snuffleupagus_globals->config_log_max_len = 255;

#define SP_INIT_HT(F)                                                          \
  snuffleupagus_globals->F = pemalloc(sizeof(*(snuffleupagus_globals->F)), 1); \
    zend_hash_init(snuffleupagus_globals->F, 10, NULL, NULL, 1);
  SP_INIT_HT(disabled_functions_hook);
  SP_INIT_HT(sp_internal_functions_hook);
  SP_INIT_HT(sp_eval_blacklist_functions_hook);
  SP_INIT_HT(config_disabled_functions);
  SP_INIT_HT(config_disabled_functions_hooked);
  SP_INIT_HT(config_disabled_functions_ret);
  SP_INIT_HT(config_disabled_functions_ret_hooked);
  SP_INIT_HT(config_ini.entries);
#undef SP_INIT_HT

#define SP_INIT_NULL(F) snuffleupagus_globals->F = NULL;
  SP_INIT_NULL(config_encryption_key);
  SP_INIT_NULL(config_cookies_env_var);
  SP_INIT_NULL(config_disabled_functions_reg.disabled_functions);
  SP_INIT_NULL(config_disabled_functions_reg_ret.disabled_functions);
  SP_INIT_NULL(config_cookie.cookies);
  SP_INIT_NULL(config_eval.blacklist);
  SP_INIT_NULL(config_eval.whitelist);
  SP_INIT_NULL(config_wrapper.whitelist);
#undef SP_INIT_NULL
}

PHP_MINIT_FUNCTION(snuffleupagus) {
  sp_log_debug("(MINIT)");
  REGISTER_INI_ENTRIES();

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(snuffleupagus) {
  sp_log_debug("(MSHUTDOWN)");
  unhook_functions(SPG(sp_internal_functions_hook));
  unhook_functions(SPG(disabled_functions_hook));
  unhook_functions(SPG(sp_eval_blacklist_functions_hook));
  if (SPCFG(ini).enable) { sp_unhook_ini(); }
  UNREGISTER_INI_ENTRIES();

  return SUCCESS;
}

static inline void free_disabled_functions_hashtable(HashTable *const ht) {
  void *ptr = NULL;
  ZEND_HASH_FOREACH_PTR(ht, ptr) { sp_list_free(ptr, sp_free_disabled_function); }
  ZEND_HASH_FOREACH_END();
}

static inline void free_config_ini_entries(HashTable *const ht) {
  void *ptr = NULL;
  ZEND_HASH_FOREACH_PTR(ht, ptr) { sp_free_ini_entry(ptr); pefree(ptr, 1); }
  ZEND_HASH_FOREACH_END();
}

static PHP_GSHUTDOWN_FUNCTION(snuffleupagus) {
  sp_log_debug("(GSHUTDOWN)");
#define FREE_HT(F)                       \
  zend_hash_destroy(snuffleupagus_globals->F); \
  pefree(snuffleupagus_globals->F, 1);
  FREE_HT(disabled_functions_hook);
  FREE_HT(sp_eval_blacklist_functions_hook);

#define FREE_HT_LIST(F)                                         \
  free_disabled_functions_hashtable(snuffleupagus_globals->F); \
  FREE_HT(F);
  FREE_HT_LIST(config_disabled_functions);
  FREE_HT_LIST(config_disabled_functions_hooked);
  FREE_HT_LIST(config_disabled_functions_ret);
  FREE_HT_LIST(config_disabled_functions_ret_hooked);
#undef FREE_HT_LIST

  free_config_ini_entries(snuffleupagus_globals->config_ini.entries);
  FREE_HT(config_ini.entries);
#undef FREE_HT

  sp_list_free(snuffleupagus_globals->config_disabled_functions_reg.disabled_functions, sp_free_disabled_function);
  sp_list_free(snuffleupagus_globals->config_disabled_functions_reg_ret.disabled_functions, sp_free_disabled_function);
  sp_list_free(snuffleupagus_globals->config_cookie.cookies, sp_free_cookie);

#define FREE_LST(L) sp_list_free(snuffleupagus_globals->L, sp_free_zstr);
  FREE_LST(config_eval.blacklist);
  FREE_LST(config_eval.whitelist);
  FREE_LST(config_wrapper.whitelist);
#undef FREE_LST


// #define FREE_CFG(C) pefree(snuffleupagus_globals->config.C, 1);
#define FREE_CFG_ZSTR(C) sp_free_zstr(snuffleupagus_globals->C);
  FREE_CFG_ZSTR(config_unserialize.dump);
  FREE_CFG_ZSTR(config_unserialize.textual_representation);
  FREE_CFG_ZSTR(config_upload_validation.script);
  FREE_CFG_ZSTR(config_eval.dump);
  FREE_CFG_ZSTR(config_eval.textual_representation);
// #undef FREE_CFG
#undef FREE_CFG_ZSTR

#ifdef SP_DEBUG_STDERR
  if (sp_debug_stderr >= 0) {
    close(sp_debug_stderr);
    sp_debug_stderr = STDERR_FILENO;
  }
#endif
}

PHP_RINIT_FUNCTION(snuffleupagus) {
  SPG(execution_depth) = 0;
  SPG(in_eval) = 0;

  const sp_config_wrapper *const config_wrapper = &(SPCFG(wrapper));
#if defined(COMPILE_DL_SNUFFLEUPAGUS) && defined(ZTS)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif

  if (!SPG(allow_broken_configuration)) {
    if (SPG(is_config_valid) == SP_CONFIG_INVALID) {
      sp_log_err("config", "Invalid configuration file");
    } else if (SPG(is_config_valid) == SP_CONFIG_NONE) {
      sp_log_warn("config", "No configuration specified via sp.configuration_file");
    }
  }

  // We need to disable wrappers loaded by extensions loaded after SNUFFLEUPAGUS.
  if (config_wrapper->enabled &&
      zend_hash_num_elements(php_stream_get_url_stream_wrappers_hash()) != config_wrapper->num_wrapper) {
    sp_disable_wrapper();
  }

  if (NULL != SPCFG(encryption_key)) {
    if (NULL != SPCFG(cookie).cookies) {
      zend_hash_apply_with_arguments(Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]), decrypt_cookie, 0);
    }
  }
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(snuffleupagus) { return SUCCESS; }

PHP_MINFO_FUNCTION(snuffleupagus) {
  const char *valid_config;
  switch (SPG(is_config_valid)) {
    case SP_CONFIG_VALID:
      valid_config = "yes";
      break;
    case SP_CONFIG_INVALID:
      valid_config = "invalid";
      break;
    case SP_CONFIG_NONE:
    default:
      valid_config = "no";
  }
  php_info_print_table_start();
  php_info_print_table_row(
      2, "snuffleupagus support",
      SPG(is_config_valid) ? "enabled" : "disabled");
  php_info_print_table_row(2, "Version", PHP_SNUFFLEUPAGUS_VERSION "-sng (with Suhosin-NG patches)");
  php_info_print_table_row(2, "Valid config", valid_config);
  php_info_print_table_end();
  DISPLAY_INI_ENTRIES();
}

#define ADD_ASSOC_ZSTR(arr, key, zstr) if (zstr) { add_assoc_str(arr, key, zstr); } else { add_assoc_null(arr, key); }
#define ADD_ASSOC_REGEXP(arr, key, regexp) if (regexp && regexp->pattern) { add_assoc_str(arr, key, regexp->pattern); } else { add_assoc_null(arr, key); }

static void add_df_to_arr(zval *arr, sp_disabled_function const *const df) {
  zval arr_df;
  array_init(&arr_df);

  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_FILENAME, df->filename);
  ADD_ASSOC_REGEXP(&arr_df, SP_TOKEN_FILENAME_REGEXP, df->r_filename);
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_FUNCTION, df->function);
  ADD_ASSOC_REGEXP(&arr_df, SP_TOKEN_FUNCTION_REGEXP, df->r_function);
  if (df->functions_list && df->functions_list->data) {
    zval arr_fl;
    array_init(&arr_fl);
    for (sp_list_node *p = df->functions_list; p; p = p->next) { add_next_index_string(&arr_fl, (char*)p->data); }
    add_assoc_zval(&arr_df, "function_list", &arr_fl);
  } else {
    add_assoc_null(&arr_df, "function_list");
  }
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_HASH, df->hash);
  add_assoc_bool(&arr_df, SP_TOKEN_SIM, df->simulation);
  if (df->param && df->param->value) {
    add_assoc_string(&arr_df, SP_TOKEN_PARAM, df->param->value);
  } else {
    add_assoc_null(&arr_df, SP_TOKEN_PARAM);
  }
  ADD_ASSOC_REGEXP(&arr_df, SP_TOKEN_PARAM_REGEXP, df->r_param);
  add_assoc_long(&arr_df, SP_TOKEN_PARAM_TYPE, df->param_type);
  add_assoc_long(&arr_df, SP_TOKEN_VALUE_ARG_POS, df->pos);
  add_assoc_long(&arr_df, SP_TOKEN_LINE_NUMBER, df->line);
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_RET, df->ret);
  ADD_ASSOC_REGEXP(&arr_df, SP_TOKEN_RET_REGEXP, df->r_ret);
  add_assoc_long(&arr_df, SP_TOKEN_RET_TYPE, df->ret_type);
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_VALUE, df->value);
  ADD_ASSOC_REGEXP(&arr_df, SP_TOKEN_VALUE_REGEXP, df->r_value);
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_KEY, df->key);
  ADD_ASSOC_REGEXP(&arr_df, SP_TOKEN_KEY_REGEXP, df->r_key);
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_DUMP, df->dump);
  ADD_ASSOC_ZSTR(&arr_df, SP_TOKEN_ALIAS, df->alias);
  add_assoc_bool(&arr_df, "param_is_array", df->param_is_array);
  add_assoc_bool(&arr_df, "var_is_array", df->var_is_array);
  add_assoc_bool(&arr_df, "allow", df->allow);
  // todo: properly traverse tree for .var() and .param()
  // sp_tree *tr = df->var;
  // for (; tr; tr = tr->next) {
  //   sp_log_debug("tree: %s", tr->value);
  // }

  if (df->var && df->var->value) {
    add_assoc_string(&arr_df, SP_TOKEN_LOCAL_VAR, df->var->value);
  } else {
    add_assoc_null(&arr_df, SP_TOKEN_LOCAL_VAR);
  }
  if (df->param && df->param->value) {
    add_assoc_string(&arr_df, SP_TOKEN_PARAM, df->param->value);
  } else {
    add_assoc_null(&arr_df, SP_TOKEN_PARAM);
  }

  if (df->cidr) {
    char cidrstr[INET6_ADDRSTRLEN+5];
    if (!get_ip_str(cidrstr, sizeof(cidrstr), df->cidr)) {
      add_assoc_null(&arr_df, SP_TOKEN_CIDR);
    } else {
      add_assoc_string(&arr_df, SP_TOKEN_CIDR, cidrstr);
    }
  } else {
    add_assoc_null(&arr_df, SP_TOKEN_CIDR);
  }

  add_next_index_zval(arr, &arr_df);
}

static void dump_config() {
  zval arr;
  php_serialize_data_t var_hash;
  smart_str buf = {0};

  array_init(&arr);
  add_assoc_string(&arr, "version", PHP_SNUFFLEUPAGUS_VERSION);

  add_assoc_bool(&arr, SP_TOKEN_UNSERIALIZE_NOCLASS "." SP_TOKEN_ENABLE, SPCFG(unserialize_noclass).enable);

  add_assoc_bool(&arr, SP_TOKEN_UNSERIALIZE_HMAC "." SP_TOKEN_ENABLE, SPCFG(unserialize).enable);
  add_assoc_bool(&arr, SP_TOKEN_UNSERIALIZE_HMAC "." SP_TOKEN_SIM, SPCFG(unserialize).simulation);
  ADD_ASSOC_ZSTR(&arr, SP_TOKEN_UNSERIALIZE_HMAC "." SP_TOKEN_DUMP, SPCFG(unserialize).dump);

  add_assoc_bool(&arr, SP_TOKEN_HARDEN_RANDOM "." SP_TOKEN_ENABLE, SPCFG(random).enable);

  add_assoc_bool(&arr, "readonly_exec.enable", SPCFG(readonly_exec).enable);
  add_assoc_bool(&arr, "readonly_exec.sim", SPCFG(readonly_exec).simulation);
  ADD_ASSOC_ZSTR(&arr, SP_TOKEN_READONLY_EXEC "." SP_TOKEN_DUMP, SPCFG(readonly_exec).dump);
  add_assoc_bool(&arr, "readonly_exec.extended_checks", SPCFG(readonly_exec).extended_checks);

  add_assoc_bool(&arr, "global_strict.enable", SPCFG(global_strict).enable);

  add_assoc_bool(&arr, "upload_validation.enable", SPCFG(upload_validation).enable);
  add_assoc_bool(&arr, "upload_validation.sim", SPCFG(upload_validation).simulation);
  ADD_ASSOC_ZSTR(&arr, SP_TOKEN_UPLOAD_VALIDATION "." SP_TOKEN_UPLOAD_SCRIPT, SPCFG(upload_validation).script);

  // global
  add_assoc_bool(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_ENCRYPTION_KEY, SPCFG(encryption_key) && ZSTR_LEN(SPCFG(encryption_key)));
  ADD_ASSOC_ZSTR(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_ENV_VAR, SPCFG(cookies_env_var));
  add_assoc_long(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_LOG_MEDIA, SPCFG(log_media));
  add_assoc_long(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_LOG_MAX_LEN, SPCFG(log_max_len));
  add_assoc_long(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_MAX_EXECUTION_DEPTH, SPCFG(max_execution_depth));
  add_assoc_bool(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_SERVER_ENCODE, SPCFG(server_encode));
  add_assoc_bool(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_SERVER_STRIP, SPCFG(server_strip));
  add_assoc_bool(&arr, SP_TOKEN_GLOBAL "." SP_TOKEN_SHOW_OLD_PHP_WARNING, SPCFG(show_old_php_warning));

  add_assoc_bool(&arr, SP_TOKEN_AUTO_COOKIE_SECURE, SPCFG(auto_cookie_secure).enable);
  add_assoc_bool(&arr, SP_TOKEN_XXE_PROTECTION, SPCFG(xxe_protection).enable);

  add_assoc_bool(&arr, SP_TOKEN_EVAL_BLACKLIST "." SP_TOKEN_SIM, SPCFG(eval).simulation);
  ADD_ASSOC_ZSTR(&arr, SP_TOKEN_EVAL_BLACKLIST "." SP_TOKEN_DUMP, SPCFG(eval).dump);
#define ADD_ASSOC_SPLIST(arr, key, splist) \
  if (splist) { \
    zval arr_sp; \
    array_init(&arr_sp); \
    for (const sp_list_node *p = splist; p; p = p->next) { add_next_index_str(&arr_sp, p->data); } \
    add_assoc_zval(arr, key, &arr_sp); \
  } else { add_assoc_null(arr, key); }

  ADD_ASSOC_SPLIST(&arr, SP_TOKEN_EVAL_BLACKLIST "." SP_TOKEN_LIST, SPCFG(eval).blacklist);
  ADD_ASSOC_SPLIST(&arr, SP_TOKEN_EVAL_WHITELIST "." SP_TOKEN_LIST, SPCFG(eval).whitelist)

  add_assoc_bool(&arr, SP_TOKEN_SESSION_ENCRYPTION "." SP_TOKEN_ENCRYPT, SPCFG(session).encrypt);
  add_assoc_bool(&arr, SP_TOKEN_SESSION_ENCRYPTION "." SP_TOKEN_SIM, SPCFG(session).simulation);

  add_assoc_long(&arr, SP_TOKEN_SESSION_ENCRYPTION "." SP_TOKEN_SID_MIN_LENGTH, SPCFG(session).sid_min_length);
  add_assoc_long(&arr, SP_TOKEN_SESSION_ENCRYPTION "." SP_TOKEN_SID_MAX_LENGTH, SPCFG(session).sid_max_length);
  add_assoc_bool(&arr, SP_TOKEN_SLOPPY_COMPARISON "." SP_TOKEN_ENABLE, SPCFG(sloppy).enable);

  ADD_ASSOC_SPLIST(&arr, SP_TOKEN_ALLOW_WRAPPERS, SPCFG(wrapper).whitelist);

#undef ADD_ASSOC_SPLIST

  add_assoc_bool(&arr, SP_TOKEN_INI_PROTECTION "." SP_TOKEN_ENABLE, SPCFG(ini).enable);
  add_assoc_bool(&arr, SP_TOKEN_INI_PROTECTION "." SP_TOKEN_SIM, SPCFG(ini).simulation);
  add_assoc_bool(&arr, SP_TOKEN_INI_PROTECTION "." "policy_ro", SPCFG(ini).policy_readonly);
  add_assoc_bool(&arr, SP_TOKEN_INI_PROTECTION "." "policy_silent_ro", SPCFG(ini).policy_silent_ro);
  add_assoc_bool(&arr, SP_TOKEN_INI_PROTECTION "." "policy_silent_fail", SPCFG(ini).policy_silent_fail);
  add_assoc_bool(&arr, SP_TOKEN_INI_PROTECTION "." "policy_drop", SPCFG(ini).policy_drop);

  if (SPCFG(ini).entries && zend_hash_num_elements(SPCFG(ini).entries) > 0) {
    zval arr_ini;
    array_init(&arr_ini);

    const sp_ini_entry *sp_entry;
    ZEND_HASH_FOREACH_PTR(SPCFG(ini).entries, sp_entry)
      zval arr_ini_entry;
      array_init(&arr_ini_entry);
      add_assoc_bool(&arr_ini_entry, SP_TOKEN_SIM, sp_entry->simulation);
      ADD_ASSOC_ZSTR(&arr_ini_entry, SP_TOKEN_KEY, sp_entry->key);
      ADD_ASSOC_ZSTR(&arr_ini_entry, "msg", sp_entry->msg);
      ADD_ASSOC_ZSTR(&arr_ini_entry, "set", sp_entry->set);
      ADD_ASSOC_ZSTR(&arr_ini_entry, "min", sp_entry->min);
      ADD_ASSOC_ZSTR(&arr_ini_entry, "max", sp_entry->max);
      add_assoc_long(&arr_ini_entry, "access", sp_entry->access);
      add_assoc_bool(&arr_ini_entry, "drop", sp_entry->drop);
      add_assoc_bool(&arr_ini_entry, "allow_null", sp_entry->allow_null);
      ADD_ASSOC_REGEXP(&arr_ini_entry, "regexp", sp_entry->regexp);
      add_next_index_zval(&arr_ini, &arr_ini_entry);
    ZEND_HASH_FOREACH_END();
    add_assoc_zval(&arr, SP_TOKEN_INI, &arr_ini);
  } else {
    add_assoc_null(&arr, SP_TOKEN_INI);
  }

  if (SPCFG(cookie).cookies && SPCFG(cookie).cookies->data) {
    zval arr_cookies;
    array_init(&arr_cookies);

    sp_cookie *cookie;
    const sp_list_node *p = SPCFG(cookie).cookies;
    for (; p; p = p->next) {
      zval arr_cookie;
      array_init(&arr_cookie);
      cookie = (sp_cookie*)p->data;

      add_assoc_long(&arr_cookie, SP_TOKEN_SAMESITE, cookie->samesite);
      add_assoc_bool(&arr_cookie, SP_TOKEN_ENCRYPT, cookie->encrypt);
      ADD_ASSOC_ZSTR(&arr_cookie, SP_TOKEN_NAME, cookie->name);
      ADD_ASSOC_REGEXP(&arr_cookie, SP_TOKEN_NAME_REGEXP, cookie->name_r);
      add_assoc_bool(&arr_cookie, SP_TOKEN_SIM, cookie->simulation);

      add_next_index_zval(&arr_cookies, &arr_cookie);
    }

    add_assoc_zval(&arr, SP_TOKEN_COOKIE_ENCRYPTION, &arr_cookies);
  } else {
    add_assoc_null(&arr, SP_TOKEN_COOKIE_ENCRYPTION);
  }

  // disabled_functions
  zval arr_dfs;
  array_init(&arr_dfs);
  size_t num_df = 0;
  const sp_list_node *dflist;
  const sp_list_node *dfp;
  ZEND_HASH_FOREACH_PTR(SPCFG(disabled_functions), dflist)
    for (dfp = dflist; dfp; dfp = dfp->next) {
      add_df_to_arr(&arr_dfs, dfp->data);
      num_df++;
    }
  ZEND_HASH_FOREACH_END();
  ZEND_HASH_FOREACH_PTR(SPCFG(disabled_functions_ret), dflist)
    for (dfp = dflist; dfp; dfp = dfp->next) {
      add_df_to_arr(&arr_dfs, dfp->data);
      num_df++;
    }
  ZEND_HASH_FOREACH_END();
  for (dfp = SPCFG(disabled_functions_reg).disabled_functions; dfp; dfp = dfp->next) {
    add_df_to_arr(&arr_dfs, dfp->data);
    num_df++;
  }
  for (dfp = SPCFG(disabled_functions_reg_ret).disabled_functions; dfp; dfp = dfp->next) {
    add_df_to_arr(&arr_dfs, dfp->data);
    num_df++;
  }

  if (num_df) {
    add_assoc_zval(&arr, SP_TOKEN_DISABLE_FUNC, &arr_dfs);
  } else {
    add_assoc_null(&arr, SP_TOKEN_DISABLE_FUNC);
  }

  // serialize and print array
  PHP_VAR_SERIALIZE_INIT(var_hash);
  php_var_serialize(&buf, &arr, &var_hash);
  PHP_VAR_SERIALIZE_DESTROY(var_hash);

  printf("%s", ZSTR_VAL(buf.s));
  sp_log_debug("--");

  smart_str_free(&buf);

}

static PHP_INI_MH(OnUpdateConfiguration) {
  TSRMLS_FETCH();

  if (!new_value || !new_value->len) {
    return FAILURE;
  }

  // set some defaults
  SPCFG(show_old_php_warning) = true;
  SPCFG(readonly_exec).extended_checks = true;

  char *str = new_value->val;

  while (1) {
    // We don't care about overwriting new_value->val
    const char *config_file = strsep(&str, ",");
    if (config_file == NULL) break;

    glob_t globbuf;
    if (0 != glob(config_file, GLOB_NOCHECK, NULL, &globbuf)) {
      SPG(is_config_valid) = SP_CONFIG_INVALID;
      globfree(&globbuf);
      return FAILURE;
    }

    for (size_t i = 0; globbuf.gl_pathv[i]; i++) {
      if (sp_parse_config(globbuf.gl_pathv[i]) != SUCCESS) {
        SPG(is_config_valid) = SP_CONFIG_INVALID;
        globfree(&globbuf);
        return FAILURE;
      }
    }
    globfree(&globbuf);
  }

  SPG(is_config_valid) = SP_CONFIG_VALID;

  // dump config
  sp_log_debug("module name? %s", sapi_module.name);
  if (getenv("SP_DUMP_CONFIG")) {
    sp_log_debug("env? %s", getenv("SP_DUMP_CONFIG"));
  }

  if (strcmp(sapi_module.name, "cli") == 0 && getenv("SP_DUMP_CONFIG")) {
    dump_config();
    return SUCCESS;
  }


  // start hooks

  if (SPCFG(sloppy).enable) {
    hook_sloppy();
  }

  if (SPCFG(random).enable) {
    hook_rand();
  }

  if (SPCFG(upload_validation).enable) {
    hook_upload();
  }

  if (SPCFG(xxe_protection).enable) {
    hook_libxml_disable_entity_loader();
  }

  if (SPCFG(wrapper).enabled) {
    hook_stream_wrappers();
  }

  if (SPCFG(session).encrypt || SPCFG(session).sid_min_length || SPCFG(session).sid_max_length) {
    hook_session();
  }

  if ((NULL != SPCFG(encryption_key) && SPCFG(unserialize).enable) || SPCFG(unserialize_noclass).enable) {
    hook_serialize();
  }

  hook_execute();
  hook_cookies();

  if (SPCFG(ini).enable) {
    sp_hook_ini();
  }

  sp_hook_register_server_variables();

  if (SPCFG(global_strict).enable) {
    if (!zend_get_extension(PHP_SNUFFLEUPAGUS_EXTNAME)) {
      zend_extension_entry.startup = NULL;
      zend_register_extension(&zend_extension_entry, NULL);
    }
    // This is needed to implement the global strict mode
    CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
  }

  hook_disabled_functions();

  // If `zend_write_default` is not NULL it is already hooked.
  if ((zend_hash_str_find(SPCFG(disabled_functions_hooked), ZEND_STRL("echo")) ||
       zend_hash_str_find(SPCFG(disabled_functions_ret_hooked), ZEND_STRL("echo"))) &&
      NULL == zend_write_default && zend_write != hook_echo) {
    zend_write_default = zend_write;
    zend_write = hook_echo;
  }

  SPG(hook_execute) = SPCFG(max_execution_depth) > 0 ||
    SPCFG(disabled_functions_reg).disabled_functions ||
    SPCFG(disabled_functions_reg_ret).disabled_functions ||
    (SPCFG(disabled_functions) && zend_hash_num_elements(SPCFG(disabled_functions))) ||
    (SPCFG(disabled_functions_ret) && zend_hash_num_elements(SPCFG(disabled_functions_ret)));

  if (SPCFG(show_old_php_warning) && getenv("SP_SKIP_OLD_PHP_CHECK") == NULL) {
    const time_t ts = time(NULL);
    if ((PHP_VERSION_ID < 70300) ||
        (PHP_VERSION_ID < 70400 && ts >= (time_t)1638745200L) ||
        (PHP_VERSION_ID < 80000 && ts >= (time_t)1669590000L) ||
        (PHP_VERSION_ID < 80100 && ts >= (time_t)1700953200L)) {
      sp_log_warn("End-of-Life Check", "Your PHP version '" PHP_VERSION "' is not officially maintained anymore. " \
		      "Please upgrade as soon as possible. - Note: This message can be switched off by setting " \
		      "'sp.global.show_old_php_warning.disable();' in your rules file or by setting the environment variable SP_SKIP_OLD_PHP_CHECK=1.");
    }
  }
  return SUCCESS;
}

const zend_function_entry snuffleupagus_functions[] = {PHP_FE_END};

zend_module_entry snuffleupagus_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_SNUFFLEUPAGUS_EXTNAME,
    snuffleupagus_functions,
    PHP_MINIT(snuffleupagus),
    PHP_MSHUTDOWN(snuffleupagus),
    PHP_RINIT(snuffleupagus),
    PHP_RSHUTDOWN(snuffleupagus),
    PHP_MINFO(snuffleupagus),
    PHP_SNUFFLEUPAGUS_VERSION,
    PHP_MODULE_GLOBALS(snuffleupagus),
    PHP_GINIT(snuffleupagus),
    PHP_GSHUTDOWN(snuffleupagus),
    NULL,
    STANDARD_MODULE_PROPERTIES_EX};

#ifdef COMPILE_DL_SNUFFLEUPAGUS
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(snuffleupagus)
#endif
