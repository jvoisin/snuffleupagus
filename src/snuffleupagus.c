#ifdef PHP_WIN32
#include "win32/glob.h"
#else
#include <glob.h>
#endif

#include "php_snuffleupagus.h"

#ifndef ZEND_EXT_API
#define ZEND_EXT_API ZEND_DLEXPORT
#endif

static PHP_INI_MH(OnUpdateConfiguration);
static inline void sp_op_array_handler(zend_op_array *op);

ZEND_EXTENSION();

// LCOV_EXCL_START
ZEND_DLEXPORT int sp_zend_startup(zend_extension *extension) {
  return zend_startup_module(&snuffleupagus_module_entry);
}
// LCOV_EXCL_END

static inline void sp_op_array_handler(zend_op_array *op) {
  if (NULL == op->filename) {
    return;
  } else {
    op->fn_flags |= ZEND_ACC_STRICT_TYPES;
  }
}

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static PHP_INI_MH(StrictMode) {
  TSRMLS_FETCH();

  SNUFFLEUPAGUS_G(allow_broken_configuration) = false;
  if (new_value && zend_string_equals_literal(new_value, "1")) {
    SNUFFLEUPAGUS_G(allow_broken_configuration) = true;
  }
  return SUCCESS;
}

PHP_INI_BEGIN()
PHP_INI_ENTRY("sp.configuration_file", "", PHP_INI_SYSTEM,
              OnUpdateConfiguration)
PHP_INI_ENTRY("sp.allow_broken_configuration", "0", PHP_INI_SYSTEM,
              StrictMode)
PHP_INI_END()

ZEND_DLEXPORT zend_extension zend_extension_entry = {
    PHP_SNUFFLEUPAGUS_EXTNAME,
    PHP_SNUFFLEUPAGUS_VERSION,
    PHP_SNUFFLEUPAGUS_AUTHOR,
    PHP_SNUFFLEUPAGUS_URL,
    PHP_SNUFFLEUPAGUS_COPYRIGHT,
    sp_zend_startup,
    NULL,
    NULL,                 /* activate_func_t */
    NULL,                 /* deactivate_func_t */
    NULL,                 /* message_handler_func_t */
    sp_op_array_handler,  // zend_global_strict, /* op_array_handler_func_t */
    NULL,                 /* statement_handler_func_t */
    NULL,                 /* fcall_begin_handler_func_t */
    NULL,                 /* fcall_end_handler_func_t */
    NULL,                 /* op_array_ctor_func_t */
    NULL,                 /* op_array_dtor_func_t */
    STANDARD_ZEND_EXTENSION_PROPERTIES};

PHP_GINIT_FUNCTION(snuffleupagus) {
  snuffleupagus_globals->is_config_valid = SP_CONFIG_NONE;
  snuffleupagus_globals->in_eval = 0;

#define SP_INIT_HT(F) snuffleupagus_globals->F = \
    pemalloc(sizeof(*(snuffleupagus_globals->F)), 1); \
    zend_hash_init(snuffleupagus_globals->F, 10, NULL, NULL, 1);
  SP_INIT_HT(disabled_functions_hook);
  SP_INIT_HT(sp_internal_functions_hook);
  SP_INIT_HT(sp_eval_blacklist_functions_hook);
  SP_INIT_HT(config.config_disabled_functions);
  SP_INIT_HT(config.config_disabled_functions_hooked);
  SP_INIT_HT(config.config_disabled_functions_ret);
  SP_INIT_HT(config.config_disabled_functions_ret_hooked);
#undef SP_INIT_HT

#define SP_INIT(F) snuffleupagus_globals->config.F = \
    pecalloc(sizeof(*(snuffleupagus_globals->config.F)), 1, 1);
  SP_INIT(config_unserialize);
  SP_INIT(config_random);
  SP_INIT(config_sloppy);
  SP_INIT(config_readonly_exec);
  SP_INIT(config_global_strict);
  SP_INIT(config_auto_cookie_secure);
  SP_INIT(config_snuffleupagus);
  SP_INIT(config_disable_xxe);
  SP_INIT(config_upload_validation);
  SP_INIT(config_disabled_functions_reg);
  SP_INIT(config_disabled_functions_reg_ret);
  SP_INIT(config_cookie);
  SP_INIT(config_session);
  SP_INIT(config_eval);
  SP_INIT(config_wrapper);
#undef SP_INIT

#define SP_INIT_NULL(F) snuffleupagus_globals->config.F = NULL;
  SP_INIT_NULL(config_disabled_functions_reg->disabled_functions);
  SP_INIT_NULL(config_disabled_functions_reg_ret->disabled_functions);
  SP_INIT_NULL(config_cookie->cookies);
  SP_INIT_NULL(config_eval->blacklist);
  SP_INIT_NULL(config_eval->whitelist);
  SP_INIT_NULL(config_wrapper->whitelist);
#undef SP_INIT_NULL
}

PHP_MINIT_FUNCTION(snuffleupagus) {
  REGISTER_INI_ENTRIES();

  return SUCCESS;
}

static void free_disabled_functions_hashtable(HashTable *ht) {
  void *ptr = NULL;
  ZEND_HASH_FOREACH_PTR(ht, ptr) { sp_list_free(ptr); }
  ZEND_HASH_FOREACH_END();
}

PHP_MSHUTDOWN_FUNCTION(snuffleupagus) {

#define FREE_HT(F)                       \
      zend_hash_destroy(SNUFFLEUPAGUS_G(F)); \
      pefree(SNUFFLEUPAGUS_G(F), 1);
  FREE_HT(disabled_functions_hook);
  FREE_HT(sp_eval_blacklist_functions_hook);

#define FREE_HT_LIST(F) \
      free_disabled_functions_hashtable(SNUFFLEUPAGUS_G(config).F); \
      FREE_HT(config.F);
  FREE_HT_LIST(config_disabled_functions);
  FREE_HT_LIST(config_disabled_functions_hooked);
  FREE_HT_LIST(config_disabled_functions_ret);
  FREE_HT_LIST(config_disabled_functions_ret_hooked);
#undef FREE_HT_LIST
#undef FREE_HT

#define FREE_LST_DISABLE(L)                \
    do {                                     \
      sp_list_node *_n = SNUFFLEUPAGUS_G(config).L; \
      sp_disabled_function_list_free(_n);    \
      sp_list_free(_n);                      \
    } while (0)
  FREE_LST_DISABLE(config_disabled_functions_reg->disabled_functions);
  FREE_LST_DISABLE(config_disabled_functions_reg_ret->disabled_functions);
#undef FREE_LST_DISABLE

#define FREE_LST(L) sp_list_free(SNUFFLEUPAGUS_G(config).L);
  FREE_LST(config_cookie->cookies);
  FREE_LST(config_eval->blacklist);
  FREE_LST(config_eval->whitelist);
  FREE_LST(config_wrapper->whitelist);
#undef FREE_LST

#define FREE_CFG(C) pefree(SNUFFLEUPAGUS_G(config).C, 1);
  FREE_CFG(config_unserialize);
  FREE_CFG(config_random);
  FREE_CFG(config_readonly_exec);
  FREE_CFG(config_global_strict);
  FREE_CFG(config_auto_cookie_secure);
  FREE_CFG(config_snuffleupagus);
  FREE_CFG(config_disable_xxe);
  FREE_CFG(config_upload_validation);
  FREE_CFG(config_session);
  FREE_CFG(config_disabled_functions_reg);
  FREE_CFG(config_disabled_functions_reg_ret);
  FREE_CFG(config_cookie);
  FREE_CFG(config_wrapper);
#undef FREE_CFG

  UNREGISTER_INI_ENTRIES();

  return SUCCESS;
}

PHP_RINIT_FUNCTION(snuffleupagus) {
  const sp_config_wrapper* config_wrapper =
      SNUFFLEUPAGUS_G(config).config_wrapper;
#if defined(COMPILE_DL_SNUFFLEUPAGUS) && defined(ZTS)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif

  if (!SNUFFLEUPAGUS_G(allow_broken_configuration)) {
    if (SNUFFLEUPAGUS_G(is_config_valid) == SP_CONFIG_INVALID ) {
      sp_log_err("config", "Invalid configuration file");
    } else if (SNUFFLEUPAGUS_G(is_config_valid) == SP_CONFIG_NONE) {
      sp_log_warn("config", "No configuration specificed via sp.configuration_file");
    }
  }

  // We need to disable wrappers loaded by extensions loaded after SNUFFLEUPAGUS.
  if (config_wrapper->enabled &&
      zend_hash_num_elements(php_stream_get_url_stream_wrappers_hash()) !=
      config_wrapper->num_wrapper) {
    sp_disable_wrapper();
  }

  if (NULL != SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key) {
    if (NULL != SNUFFLEUPAGUS_G(config).config_cookie->cookies) {
      zend_hash_apply_with_arguments(
          Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]), decrypt_cookie, 0);
    }
  }
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(snuffleupagus) { return SUCCESS; }

PHP_MINFO_FUNCTION(snuffleupagus) {
  const char *valid_config;
  switch(SNUFFLEUPAGUS_G(is_config_valid)) {
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
  php_info_print_table_row(2, "snuffleupagus support",
      SNUFFLEUPAGUS_G(is_config_valid)?"enabled":"disabled");
  php_info_print_table_row(2, "Version", PHP_SNUFFLEUPAGUS_VERSION);
  php_info_print_table_row( 2, "Valid config", valid_config);
  php_info_print_table_end();
  DISPLAY_INI_ENTRIES();
}

static PHP_INI_MH(OnUpdateConfiguration) {
  TSRMLS_FETCH();

  if (!new_value || !new_value->len) {
    return FAILURE;
  }

  glob_t globbuf;
  char *config_file;
  char *rest = new_value->val;

  while ((config_file = strtok_r(rest, ",", &rest))) {
    int ret = glob(config_file, GLOB_NOCHECK, NULL, &globbuf);

    if (ret != 0) {
      SNUFFLEUPAGUS_G(is_config_valid) = SP_CONFIG_INVALID;
      globfree(&globbuf);
      return FAILURE;
    }

    for (size_t i = 0; globbuf.gl_pathv[i]; i++) {
      if (sp_parse_config(globbuf.gl_pathv[i]) != SUCCESS) {
        SNUFFLEUPAGUS_G(is_config_valid) = SP_CONFIG_INVALID;
        globfree(&globbuf);
        return FAILURE;
      }
    }
    globfree(&globbuf);
  }

  SNUFFLEUPAGUS_G(is_config_valid) = SP_CONFIG_VALID;

  if ((SNUFFLEUPAGUS_G(config).config_sloppy->enable)) {
    hook_sloppy();
  }

  if (SNUFFLEUPAGUS_G(config).config_random->enable) {
    hook_rand();
  }

  if (SNUFFLEUPAGUS_G(config).config_upload_validation->enable) {
    hook_upload();
  }

  if (SNUFFLEUPAGUS_G(config).config_disable_xxe->enable == 0) {
    hook_libxml_disable_entity_loader();
  }

  if (SNUFFLEUPAGUS_G(config).config_wrapper->enabled) {
    hook_stream_wrappers();
  }

  if (SNUFFLEUPAGUS_G(config).config_session->encrypt) {
    hook_session();
  }

  if (NULL != SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key) {
    if (SNUFFLEUPAGUS_G(config).config_unserialize->enable) {
      hook_serialize();
    }
  }

  hook_disabled_functions();
  hook_execute();
  hook_cookies();

  if (true == SNUFFLEUPAGUS_G(config).config_global_strict->enable) {
    if (!zend_get_extension(PHP_SNUFFLEUPAGUS_EXTNAME)) {
      zend_extension_entry.startup = NULL;
      zend_register_extension(&zend_extension_entry, NULL);
    }
    // This is needed to implement the global strict mode
    CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
  }

  // If `zend_write_default` is not NULL it is already hooked.
  if ((zend_hash_str_find(
          SNUFFLEUPAGUS_G(config).config_disabled_functions_hooked, "echo",
          sizeof("echo") - 1) ||
      zend_hash_str_find(
          SNUFFLEUPAGUS_G(config).config_disabled_functions_ret_hooked, "echo",
          sizeof("echo") - 1)) && NULL == zend_write_default) {
    zend_write_default = zend_write;
    zend_write = hook_echo;
  }

  SNUFFLEUPAGUS_G(config).hook_execute =
      SNUFFLEUPAGUS_G(config)
          .config_disabled_functions_reg->disabled_functions ||
      SNUFFLEUPAGUS_G(config)
          .config_disabled_functions_reg_ret->disabled_functions ||
      zend_hash_num_elements(
          SNUFFLEUPAGUS_G(config).config_disabled_functions) ||
      zend_hash_num_elements(
          SNUFFLEUPAGUS_G(config).config_disabled_functions_ret);

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
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX};

#ifdef COMPILE_DL_SNUFFLEUPAGUS
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(snuffleupagus)
#endif
