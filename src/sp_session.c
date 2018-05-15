#include "php_snuffleupagus.h"
#include "ext/session/php_session.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus);

#ifdef ZTS
static ts_rsrc_id session_globals_id = 0;
#define SESSION_G(v) ZEND_TSRMG(session_globals_id, php_ps_globals *, v)
# ifdef COMPILE_DL_SESSION
ZEND_TSRMLS_CACHE_EXTERN();
# endif
#else
static php_ps_globals *session_globals = NULL;
#define SESSION_G(v) (ps_globals.v)
#endif

static int (*previous_sessionRINIT)(INIT_FUNC_ARGS) = NULL;
static ZEND_INI_MH((*old_OnUpdateSaveHandler)) = NULL;


// PS_READ_ARGS => void **mod_data, zend_string *key, zend_string **val, zend_long maxlifetime
static int sp_hook_s_read(PS_READ_ARGS)
{
  //zend_string *new_key = key;
  /* protect session vars */
/*  if (SESSION_G(http_session_vars) && SESSION_G(http_session_vars)->type == IS_ARRAY) {
    SESSION_G(http_session_vars)->refcount++;
  }*/

  /* protect dumb session handlers */
  /*if (COND_DUMB_SH) {
regenerate:
    SDEBUG("regenerating key. old key was %s", key ? ZSTR_VAL(key) : "<NULL>");
    zend_string_release(SESSION_G(id));
    new_key = SESSION_G(id) = SESSION_G(mod)->s_create_sid(&SESSION_G(mod_data));
    suhosin_send_cookie();
  } else if (ZSTR_LEN(key) > SNUFFLEUPAGUS_G(session_max_id_length)) {
    suhosin_log(S_SESSION, "session id ('%s') exceeds maximum length - regenerating", ZSTR_VAL(key));
    if (!SNUFFLEUPAGUS_G(simulation)) {
      goto regenerate;
    }
  }*/
  int r = SNUFFLEUPAGUS_G(old_s_read)(mod_data, key, val, maxlifetime);
  sp_log_err("debug","Decode success : %d for : %s \n", r, ZSTR_VAL(*val));

  /*if (r == SUCCESS && SNUFFLEUPAGUS_G(config).config_cookie_session->encrypt && val != NULL && *val != NULL && ZSTR_LEN(*val)) {
    char cryptkey[33];

    // SNUFFLEUPAGUS_G(do_not_scan) = 1;
    S7_GENERATE_KEY(session, cryptkey);

    zend_string *orig_val = *val;
    *val = suhosin_decrypt_string(ZSTR_VAL(*val), ZSTR_LEN(*val), "", 0, (char *)cryptkey, SNUFFLEUPAGUS_G(session_checkraddr));
    // SNUFFLEUPAGUS_G(do_not_scan) = 0;
    if (*val == NULL) {
      *val = ZSTR_EMPTY_ALLOC();
    }
    zend_string_release(orig_val);
  }*/

  return r;
}

// PS_WRITE_ARGS => void **mod_data, zend_string *key, zend_string *val, zend_long maxlifetime
static int sp_hook_s_write(PS_WRITE_ARGS)
{
  //sp_log_err("debug","Write Val : %s\n", ZSTR_VAL(val));
  /* protect dumb session handlers */
  /*if (COND_DUMB_SH) {
    return FAILURE;
  }*/

  /*if (ZSTR_LEN(val) > 0 && SNUFFLEUPAGUS_G(session_encrypt)) {
    char cryptkey[33];
    // SNUFFLEUPAGUS_G(do_not_scan) = 1;
    S7_GENERATE_KEY(session, cryptkey);

    zend_string *v = suhosin_encrypt_string(ZSTR_VAL(val), ZSTR_LEN(val), "", 0, cryptkey);

    // SNUFFLEUPAGUS_G(do_not_scan) = 0;
    return SNUFFLEUPAGUS_G(old_s_write)(mod_data, key, v, maxlifetime);
  }*/

  return SNUFFLEUPAGUS_G(old_s_write)(mod_data, key, val, maxlifetime);
}



static void sp_hook_session_module()
{
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
  //sp_log_err("debuug", "Old s_read : %p \n", mod->s_read);
  mod->s_read = sp_hook_s_read;
  //sp_log_err("debuug", "New s_read : %p \n", mod->s_read);
  //sp_log_err("debuug", "Old s_write : %p \n", mod->s_write);
  SNUFFLEUPAGUS_G(old_s_write) = mod->s_write;
  mod->s_write = sp_hook_s_write;
  //sp_log_err("debuug", "New s_write : %p \n", mod->s_write);

  SESSION_G(mod) = mod;
}


static PHP_INI_MH(sp_OnUpdateSaveHandler)
{
  if (stage == PHP_INI_STAGE_RUNTIME
    && SESSION_G(session_status) == php_session_none
    && SNUFFLEUPAGUS_G(s_original_mod)
    && zend_string_equals_literal(new_value, "user") == 0
    && strcmp(((ps_module*)SNUFFLEUPAGUS_G(s_original_mod))->s_name, "user") == 0) {
    return SUCCESS;
  }

  SESSION_G(mod) = SNUFFLEUPAGUS_G(s_original_mod);

  int r = old_OnUpdateSaveHandler(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);

  //sp_hook_session_module();

  return r;
}

static int sp_hook_session_RINIT(INIT_FUNC_ARGS) {
  sp_log_err("debug","RINIT \n");
  if (SESSION_G(mod) == NULL) {
    zend_ini_entry *ini_entry;
    if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives), ZEND_STRL("session.save_handler")))) {
      if (ini_entry->value) {
        sp_OnUpdateSaveHandler(NULL, ini_entry->value, NULL, NULL, NULL, 0);
      }
    }
  }
  sp_log_err("debug","Return old session RINIT\n");
  return previous_sessionRINIT(INIT_FUNC_ARGS_PASSTHRU);
}

void hook_session() {
  zend_module_entry *module;
  sp_log_err("debug","First hook\n");
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
  /*if (old_OnUpdateSaveHandler != NULL) {
    return;
  }*/

  previous_sessionRINIT = module->request_startup_func;
  module->request_startup_func = sp_hook_session_RINIT;

  zend_ini_entry *ini_entry;
  if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives), ZEND_STRL("session.save_handler"))) != NULL ) {
    old_OnUpdateSaveHandler = ini_entry->on_modify;
    ini_entry->on_modify = sp_OnUpdateSaveHandler;
  }
  SNUFFLEUPAGUS_G(s_module) = NULL;

  sp_hook_session_module();

}