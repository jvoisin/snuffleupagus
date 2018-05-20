#include "php_snuffleupagus.h"
#include "ext/session/php_session.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus);

#ifdef ZTS
static ts_rsrc_id session_globals_id = 0;
#define SESSION_G(v) ZEND_TSRMG(session_globals_id, php_ps_globals *, v)
#ifdef COMPILE_DL_SESSION
ZEND_TSRMLS_CACHE_EXTERN();
#endif
#else
static php_ps_globals *session_globals = NULL;
#define SESSION_G(v) (ps_globals.v)
#endif

static int (*previous_sessionRINIT)(INIT_FUNC_ARGS) = NULL;
static ZEND_INI_MH((*old_OnUpdateSaveHandler)) = NULL;

// PS_READ_ARGS => void **mod_data, zend_string *key, zend_string **val,
// zend_long maxlifetime
static int sp_hook_s_read(PS_READ_ARGS) {
  int r = SNUFFLEUPAGUS_G(old_s_read)(mod_data, key, val, maxlifetime);
  if (r == SUCCESS && SNUFFLEUPAGUS_G(config).config_session->encrypt &&
      val != NULL && *val != NULL && ZSTR_LEN(*val)) {
    zend_string *orig_val = *val;
    zval val_zval;
    ZVAL_PSTRINGL(&val_zval, ZSTR_VAL(*val), ZSTR_LEN(*val));

    int ret = decrypt_zval(
        &val_zval, SNUFFLEUPAGUS_G(config).config_session->simulation,
        NULL);  // NULL for the moment
    if (0 != ret) {
      if (SNUFFLEUPAGUS_G(config).config_session->simulation) {
        return ret;
      } else {
        sp_terminate();
      }
    }

    *val = zend_string_dup(val_zval.value.str, 0);
    if (*val == NULL) {
      *val = ZSTR_EMPTY_ALLOC();
    }
    zend_string_release(orig_val);
  }

  return r;
}

// PS_WRITE_ARGS => void **mod_data, zend_string *key, zend_string *val,
// zend_long maxlifetime
static int sp_hook_s_write(PS_WRITE_ARGS) {
  if (ZSTR_LEN(val) > 0 &&
      SNUFFLEUPAGUS_G(config).config_session->encrypt) {
    zend_string *new_val = encrypt_zval(ZSTR_VAL(val), ZSTR_LEN(val));
    return SNUFFLEUPAGUS_G(old_s_write)(mod_data, key, new_val, maxlifetime);
  }
  return SNUFFLEUPAGUS_G(old_s_write)(mod_data, key, val, maxlifetime);
}

static void sp_hook_session_module() {
  ps_module *old_mod = SESSION_G(mod);
  ps_module *mod;

  if (old_mod == NULL || SNUFFLEUPAGUS_G(s_module) == old_mod) {
    return;
  }

  if (SNUFFLEUPAGUS_G(s_module) == NULL) {
    SNUFFLEUPAGUS_G(s_module) = mod = malloc(sizeof(ps_module));
    if (mod == NULL) {
      return;
    }
  }

  SNUFFLEUPAGUS_G(s_original_mod) = old_mod;

  mod = SNUFFLEUPAGUS_G(s_module);
  memcpy(mod, old_mod, sizeof(ps_module));

  SNUFFLEUPAGUS_G(old_s_read) = mod->s_read;
  mod->s_read = sp_hook_s_read;

  SNUFFLEUPAGUS_G(old_s_write) = mod->s_write;
  mod->s_write = sp_hook_s_write;

  SESSION_G(mod) = mod;
}

static PHP_INI_MH(sp_OnUpdateSaveHandler) {
  if (stage == PHP_INI_STAGE_RUNTIME &&
      SESSION_G(session_status) == php_session_none &&
      SNUFFLEUPAGUS_G(s_original_mod) &&
      zend_string_equals_literal(new_value, "user") == 0 &&
      strcmp(((ps_module *)SNUFFLEUPAGUS_G(s_original_mod))->s_name, "user") ==
          0) {
    return SUCCESS;
  }

  SESSION_G(mod) = SNUFFLEUPAGUS_G(s_original_mod);

  int r = old_OnUpdateSaveHandler(entry, new_value, mh_arg1, mh_arg2, mh_arg3,
                                  stage);

  sp_hook_session_module();

  return r;
}

static int sp_hook_session_RINIT(INIT_FUNC_ARGS) {
  if (SESSION_G(mod) == NULL) {
    zend_ini_entry *ini_entry;
    if ((ini_entry = zend_hash_str_find_ptr(
             EG(ini_directives), ZEND_STRL("session.save_handler")))) {
      if (ini_entry->value) {
        sp_OnUpdateSaveHandler(NULL, ini_entry->value, NULL, NULL, NULL, 0);
      }
    }
  }
  return previous_sessionRINIT(INIT_FUNC_ARGS_PASSTHRU);
}

void hook_session() {
  zend_module_entry *module;

  if ((module = zend_hash_str_find_ptr(&module_registry,
                                       ZEND_STRL("session"))) == NULL) {
    return;
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
    return;
  }

  previous_sessionRINIT = module->request_startup_func;
  module->request_startup_func = sp_hook_session_RINIT;

  zend_ini_entry *ini_entry;
  if ((ini_entry = zend_hash_str_find_ptr(
           EG(ini_directives), ZEND_STRL("session.save_handler"))) != NULL) {
    old_OnUpdateSaveHandler = ini_entry->on_modify;
    ini_entry->on_modify = sp_OnUpdateSaveHandler;
  }
  SNUFFLEUPAGUS_G(s_module) = NULL;

  sp_hook_session_module();
}