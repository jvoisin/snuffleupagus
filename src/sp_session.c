#include "php_snuffleupagus.h"

#ifdef ZTS
static ts_rsrc_id session_globals_id = 0;
#define SESSION_G(v) ZEND_TSRMG(session_globals_id, php_ps_globals *, v)
#ifdef COMPILE_DL_SESSION
ZEND_TSRMLS_CACHE_EXTERN();
#endif
#else
static php_ps_globals *session_globals = NULL;
#define SESSION_G(v) (session_globals->v)
#endif

static ps_module *s_module;
#if PHP_VERSION_ID < 70300
static ps_module *s_original_mod;
#else
static const ps_module *s_original_mod;
#endif
static int (*old_s_read)(PS_READ_ARGS);
static int (*old_s_write)(PS_WRITE_ARGS);
static int (*previous_sessionRINIT)(INIT_FUNC_ARGS) = NULL;
static ZEND_INI_MH((*old_OnUpdateSaveHandler)) = NULL;

static void check_sid_length(zend_string *sid) {
  const sp_config_session *cfg = &(SPCFG(session));

  if (sid) {
    if (cfg->sid_min_length && ZSTR_LEN(sid) < cfg->sid_min_length) {
      sp_log_auto("session", cfg->simulation, "Session ID is too short");
    }
    if (cfg->sid_max_length && ZSTR_LEN(sid) > cfg->sid_max_length) {
      sp_log_auto("session", cfg->simulation, "Session ID is too long");
    }
  }
}

static int sp_hook_s_read(PS_READ_ARGS) {
  const sp_config_session *cfg = &(SPCFG(session));
  check_sid_length(key);

  int r = old_s_read(mod_data, key, val, maxlifetime);

  if ((NULL == val) || (NULL == *val) || (0 == ZSTR_LEN(*val))) {
    return r;
  }

  if (r == SUCCESS && cfg->encrypt) {
    zend_string *orig_val = *val;
    zval val_zval;
    ZVAL_PSTRINGL(&val_zval, ZSTR_VAL(*val), ZSTR_LEN(*val));

    int ret = decrypt_zval(&val_zval, cfg->simulation, NULL);
    if (ZEND_HASH_APPLY_KEEP != ret) {
      zend_bailout();
    }

    *val = zend_string_dup(val_zval.value.str, 0);
    zend_string_release(orig_val);
  }

  return r;
}

static int sp_hook_s_write(PS_WRITE_ARGS) {
  const sp_config_session *cfg = &(SPCFG(session));
  check_sid_length(key);

  if (ZSTR_LEN(val) > 0 && cfg->encrypt) {
    zend_string *new_val = encrypt_zval(val);
    return old_s_write(mod_data, key, new_val, maxlifetime);
  }
  return old_s_write(mod_data, key, val, maxlifetime);
}

static void sp_hook_session_module() {
#if PHP_VERSION_ID < 70300
  ps_module *old_mod = SESSION_G(mod);
#else
  const ps_module *old_mod = SESSION_G(mod);
#endif
  ps_module *mod;

  if (old_mod == NULL || s_module == old_mod) {
    return;  // LCOV_EXCL_LINE
  }

  if (s_module == NULL) {
    s_module = mod = malloc(sizeof(ps_module));
    if (mod == NULL) {
      return;  // LCOV_EXCL_LINE
    }
  }

  s_original_mod = old_mod;

  mod = s_module;
  memcpy(mod, old_mod, sizeof(ps_module));

  old_s_read = mod->s_read;
  mod->s_read = sp_hook_s_read;

  old_s_write = mod->s_write;
  mod->s_write = sp_hook_s_write;

  SESSION_G(mod) = mod;
}

static PHP_INI_MH(sp_OnUpdateSaveHandler) {
#if PHP_VERSION_ID < 70100
  /* PHP7.0 doesn't handle well recusively set session handlers */
  if (stage == PHP_INI_STAGE_RUNTIME &&
      SESSION_G(session_status) == php_session_none && s_original_mod &&
      zend_string_equals_literal(new_value, "user") == 0 &&
      strcmp(((ps_module *)s_original_mod)->s_name, "user") == 0) {
    return SUCCESS;
  }
#endif

  SESSION_G(mod) = s_original_mod;

  int r = old_OnUpdateSaveHandler(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);

  sp_hook_session_module();

  return r;
}

static int sp_hook_session_RINIT(INIT_FUNC_ARGS) {
  int ret = previous_sessionRINIT(INIT_FUNC_ARGS_PASSTHRU);
  sp_hook_session_module();
  return ret;
}

void hook_session() {
  zend_module_entry *module;

  if ((module = zend_hash_str_find_ptr(&module_registry, ZEND_STRL("session"))) == NULL) {
    sp_log_err("session", "You are trying to use session encryption or session ID restrictions, but your PHP installation has no session support. Please install the PHP session module or recompile PHP with session support.");
    return;  // LCOV_EXCL_LINE
  }

#ifdef ZTS
  if (session_globals_id == 0) {
    session_globals_id = *module->globals_id_ptr;
  }
#else
  if (session_globals == NULL) {
    session_globals = module->globals_ptr;
  }
#endif
  if (old_OnUpdateSaveHandler != NULL) {
    return;  // LCOV_EXCL_LINE
  }

  previous_sessionRINIT = module->request_startup_func;
  module->request_startup_func = sp_hook_session_RINIT;

  zend_ini_entry *ini_entry;
  if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives), ZEND_STRL("session.save_handler"))) != NULL) {
    old_OnUpdateSaveHandler = ini_entry->on_modify;
    ini_entry->on_modify = sp_OnUpdateSaveHandler;
  }
  s_module = NULL;

  sp_hook_session_module();
}
