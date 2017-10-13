#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_snuffleupagus.h"

#ifndef ZEND_EXT_API
#define ZEND_EXT_API ZEND_DLEXPORT
#endif

static PHP_INI_MH(OnUpdateConfiguration);
static inline void sp_op_array_handler(zend_op_array *op);

ZEND_EXTENSION();

ZEND_DLEXPORT int sp_zend_startup(zend_extension *extension) {
  return zend_startup_module(&snuffleupagus_module_entry);
}

static inline void sp_op_array_handler(zend_op_array *op) {
  if (NULL == op->filename) {
    return;
  } else {
    op->fn_flags |= ZEND_ACC_STRICT_TYPES;
  }
}

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

PHP_INI_BEGIN()
PHP_INI_ENTRY("sp.configuration_file", "", PHP_INI_SYSTEM,
  OnUpdateConfiguration)
PHP_INI_END()

ZEND_DLEXPORT zend_extension zend_extension_entry = {
    PHP_SNUFFLEUPAGUS_EXTNAME,
    PHP_SNUFFLEUPAGUS_VERSION,
    PHP_SNUFFLEUPAGUS_AUTHOR,
    PHP_SNUFFLEUPAGUS_URL,
    PHP_SNUFFLEUPAGUS_COPYRIGHT,
    sp_zend_startup,
    NULL,
    NULL,               /* activate_func_t */
    NULL,               /* deactivate_func_t */
    NULL,               /* message_handler_func_t */
    sp_op_array_handler,//zend_global_strict, /* op_array_handler_func_t */
    NULL,               /* statement_handler_func_t */
    NULL,               /* fcall_begin_handler_func_t */
    NULL,               /* fcall_end_handler_func_t */
    NULL,               /* op_array_ctor_func_t */
    NULL,               /* op_array_dtor_func_t */
    STANDARD_ZEND_EXTENSION_PROPERTIES};

/* Uncomment this function if you have INI entries
static void php_snuffleupagus_init_globals(zend_snuffleupagus_globals
*snuffleupagus_globals)
{
        snuffleupagus_globals->global_value = 0;
        snuffleupagus_globals->global_string = NULL;
}
*/

PHP_GINIT_FUNCTION(snuffleupagus) {
#define SP_INIT(F) F = pecalloc(sizeof(*F), 1, 1);
#define SP_INIT_HT(F) \
	F = pemalloc(sizeof(*F), 1); \
	zend_hash_init(F, 10, NULL, NULL, 1);

  SP_INIT_HT(snuffleupagus_globals->disabled_functions_hook);
  SP_INIT_HT(snuffleupagus_globals->sp_internal_functions_hook);

  SP_INIT(snuffleupagus_globals->config.config_unserialize);
  SP_INIT(snuffleupagus_globals->config.config_random);
  SP_INIT(snuffleupagus_globals->config.config_readonly_exec);
  SP_INIT(snuffleupagus_globals->config.config_global_strict);
  SP_INIT(snuffleupagus_globals->config.config_auto_cookie_secure);
  SP_INIT(snuffleupagus_globals->config.config_snuffleupagus);
  SP_INIT(snuffleupagus_globals->config.config_disable_xxe);
  SP_INIT(snuffleupagus_globals->config.config_upload_validation);
  SP_INIT(snuffleupagus_globals->config.config_disabled_functions);
  SP_INIT(snuffleupagus_globals->config.config_disabled_functions_ret);
  SP_INIT(snuffleupagus_globals->config.config_cookie_encryption);
  SP_INIT(snuffleupagus_globals->config.config_disabled_constructs);

  snuffleupagus_globals->config.config_disabled_constructs->construct_include = sp_new_list();
  snuffleupagus_globals->config.config_disabled_functions->disabled_functions = sp_new_list();
  snuffleupagus_globals->config.config_disabled_functions_ret->disabled_functions = sp_new_list();

  SP_INIT_HT(snuffleupagus_globals->config.config_cookie_encryption->names);

#undef SP_INIT
#undef SP_INIT_HT
}

PHP_MINIT_FUNCTION(snuffleupagus) {
  REGISTER_INI_ENTRIES();

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(snuffleupagus) {
#define FREE_HT(F) \
  zend_hash_destroy(SNUFFLEUPAGUS_G(F)); \
  pefree(SNUFFLEUPAGUS_G(F), 1);

  FREE_HT(disabled_functions_hook);
  FREE_HT(config.config_cookie_encryption->names);

#undef FREE_HT

  pefree(SNUFFLEUPAGUS_G(config.config_unserialize), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_random), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_readonly_exec), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_global_strict), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_auto_cookie_secure), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_snuffleupagus), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_disable_xxe), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_upload_validation), 1);
  pefree(SNUFFLEUPAGUS_G(config.config_cookie_encryption), 1);

  sp_list_free(SNUFFLEUPAGUS_G(config.config_disabled_functions->disabled_functions));
  pefree(SNUFFLEUPAGUS_G(config.config_disabled_functions), 1);
  sp_list_free(SNUFFLEUPAGUS_G(config.config_disabled_functions_ret->disabled_functions));
  pefree(SNUFFLEUPAGUS_G(config.config_disabled_functions_ret), 1);

  UNREGISTER_INI_ENTRIES();

  return SUCCESS;
}

PHP_RINIT_FUNCTION(snuffleupagus) {
#if defined(COMPILE_DL_SNUFFLEUPAGUS) && defined(ZTS)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  if (NULL != SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key) {
    if (NULL != SNUFFLEUPAGUS_G(config).config_cookie_encryption->names) {
      zend_hash_apply_with_arguments(
          Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]), decrypt_cookie, 0);
    }
  }
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(snuffleupagus) { return SUCCESS; }

PHP_MINFO_FUNCTION(snuffleupagus) {
  php_info_print_table_start();
  php_info_print_table_header(2, "snuffleupagus support", "enabled");
  php_info_print_table_end();

  DISPLAY_INI_ENTRIES();
}

static PHP_INI_MH(OnUpdateConfiguration) {
  TSRMLS_FETCH();

  if (!new_value || !new_value->len) {
    return FAILURE;
  }

  if (sp_parse_config(new_value->val) != SUCCESS) {
    return FAILURE;
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
  hook_disabled_functions();
  hook_execute();

  if (NULL != SNUFFLEUPAGUS_G(config).config_snuffleupagus->encryption_key) {
    if (SNUFFLEUPAGUS_G(config).config_unserialize->enable) {
      hook_serialize();
    }
    hook_cookies();
  }

  if (true == SNUFFLEUPAGUS_G(config).config_global_strict->enable) {
    if (!zend_get_extension(PHP_SNUFFLEUPAGUS_EXTNAME)) {
      zend_extension_entry.startup = NULL;
      zend_register_extension(&zend_extension_entry, NULL);
    }
    // This is needed to implement the global strict mode
    CG(compiler_options) |= ZEND_COMPILE_HANDLE_OP_ARRAY;
  }

  return SUCCESS;
}

const zend_function_entry snuffleupagus_functions[] = {PHP_FE_END};

zend_module_entry snuffleupagus_module_entry =
    {STANDARD_MODULE_HEADER,
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
