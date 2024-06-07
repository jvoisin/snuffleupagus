#include "php_snuffleupagus.h"

#define LOG_FEATURE "wrappers_whitelist"

static bool wrapper_is_whitelisted(const zend_string *const zs) {
  const sp_list_node *list = SPCFG(wrapper).whitelist;

  if (!zs) {
    return false;  // LCOV_EXCL_LINE
  }

  while (list) {
    if (zend_string_equals_ci(zs, (const zend_string *)list->data)) {
      return true;
    }
    list = list->next;
  }
  return false;
}

static bool sp_php_stream_is_filtered(void) {
  const sp_list_node *list = SPCFG(wrapper).php_stream_allowlist;

  return list != NULL;
}

static bool sp_php_stream_is_whitelisted(const char *const kind) {
  const sp_list_node *list = SPCFG(wrapper).php_stream_allowlist;

  while (list) {
    if (!strcasecmp(kind, ZSTR_VAL((const zend_string *)list->data))) {
      return true;
    }
    list = list->next;
  }
  return false;
}

/*
 * Adopted from
 * https://github.com/php/php-src/blob/8896bd3200892000d8aaa01595d6c64b926a26f7/ext/standard/php_fopen_wrapper.c#L176
 */
static php_stream * sp_php_stream_url_wrap_php(php_stream_wrapper *wrapper,
                                            const char *path, const char *mode,
                                            int options, zend_string **opened_path,
                                            php_stream_context *context STREAMS_DC) {
  if (!strncasecmp(path, "php://", 6)) {
    path += 6;
  }

  if (!strncasecmp(path, "temp", 4)) {
    if (!sp_php_stream_is_whitelisted("temp")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"temp\" dropped");
      return NULL;
    }
  } else if (!strcasecmp(path, "memory")) {
    if (!sp_php_stream_is_whitelisted("memory")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"memory\" dropped");
      return NULL;
    }
  } else if (!strcasecmp(path, "output")) {
    if (!sp_php_stream_is_whitelisted("output")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"output\" dropped");
      return NULL;
    }
  } else if (!strcasecmp(path, "input")) {
    if (!sp_php_stream_is_whitelisted("input")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"input\" dropped");
      return NULL;
    }
  } else if (!strcasecmp(path, "stdin")) {
    if (!sp_php_stream_is_whitelisted("stdin")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"stdin\" dropped");
      return NULL;
    }
  } else if (!strcasecmp(path, "stdout")) {
    if (!sp_php_stream_is_whitelisted("stdout")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"stdout\" dropped");
      return NULL;
    }
  } else if (!strcasecmp(path, "stderr")) {
    if (!sp_php_stream_is_whitelisted("stderr")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"stderr\" dropped");
      return NULL;
    }
  } else if (!strncasecmp(path, "fd/", 3)) {
    if (!sp_php_stream_is_whitelisted("fd")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"fd\" dropped");
      return NULL;
    }
  } else if (!strncasecmp(path, "filter/", 7)) {
    if (!sp_php_stream_is_whitelisted("filter")) {
      sp_log_warn(LOG_FEATURE, "Call to not allowed php stream type \"filter\" dropped");
      return NULL;
    }
  } else {
    sp_log_warn(LOG_FEATURE, "Call to unknown php stream type dropped");
    return NULL;
  }

  extern PHPAPI const php_stream_wrapper php_stream_php_wrapper;

  return php_stream_php_wrapper.wops->stream_opener(wrapper, path, mode, options, opened_path, context STREAMS_DC);
}

/*
 * Adopted from
 * https://github.com/php/php-src/blob/8896bd3200892000d8aaa01595d6c64b926a26f7/ext/standard/php_fopen_wrapper.c#L428-L446
 */
static const php_stream_wrapper_ops sp_php_stdio_wops = {
  sp_php_stream_url_wrap_php,
  NULL, /* close */
  NULL, /* fstat */
  NULL, /* stat */
  NULL, /* opendir */
  "PHP",
  NULL, /* unlink */
  NULL, /* rename */
  NULL, /* mkdir */
  NULL, /* rmdir */
  NULL
};
static const php_stream_wrapper sp_php_stream_php_wrapper = {
  &sp_php_stdio_wops,
  NULL,
  0, /* is_url */
};

static void sp_reregister_php_wrapper(void) {
  if (!sp_php_stream_is_filtered()) {
    return;
  }

  if (php_unregister_url_stream_wrapper("php") != SUCCESS) {
    sp_log_warn(LOG_FEATURE, "Failed to unregister stream wrapper \"php\"");
    return;
  }

  if (php_register_url_stream_wrapper("php", &sp_php_stream_php_wrapper) != SUCCESS) {
    sp_log_warn(LOG_FEATURE, "Failed to register custom stream wrapper \"php\"");
    return;
  }

  sp_log_debug(LOG_FEATURE, "Stream \"php\" successfully re-registered");
}

void sp_disable_wrapper() {
  HashTable *orig = php_stream_get_url_stream_wrappers_hash();
  HashTable *orig_complete = pemalloc(sizeof(HashTable), 1);
  zval *zv;
  zend_string *zs;

  // Copy the original hashtable into a temporary one, as I'm not sure about
  // the behaviour of ZEND_HASH_FOREACH when element are removed from the
  // hashtable used in the loop.
  zend_hash_init(orig_complete, zend_hash_num_elements(orig), NULL, NULL, 1);
  zend_hash_copy(orig_complete, orig, NULL);
  zend_hash_clean(orig);

  ZEND_HASH_FOREACH_STR_KEY_VAL(orig_complete, zs, zv) {
    if (wrapper_is_whitelisted(zs)) {
      zend_hash_add(orig, zs, zv);
    }
  }
  ZEND_HASH_FOREACH_END();

  zend_hash_destroy(orig_complete);
  pefree(orig_complete, 1);
  SPCFG(wrapper).num_wrapper = zend_hash_num_elements(orig);
}

PHP_FUNCTION(sp_stream_wrapper_register) {
  zif_handler orig_handler;
  zend_string *protocol_name = NULL;
  zval *params = NULL;
  uint32_t param_count = 0;

  zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "S*", &protocol_name, &params, &param_count);
  // ignore proper arguments here and just let the original handler deal with it
  if (!protocol_name || wrapper_is_whitelisted(protocol_name)) {

    // reject manual loading of "php" wrapper
    if (!strcasecmp(ZSTR_VAL(protocol_name), "php") && sp_php_stream_is_filtered()) {
      return;
    }

    orig_handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("stream_wrapper_register"));
    orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  }
}

PHP_FUNCTION(sp_stream_wrapper_restore) {
  zif_handler orig_handler;
  zend_string *protocol_name = NULL;
  zval *params = NULL;
  uint32_t param_count = 0;

  zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "S*", &protocol_name, &params, &param_count);
  orig_handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("stream_wrapper_restore"));
  orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

  if (protocol_name && !strcasecmp(ZSTR_VAL(protocol_name), "php")) {
    sp_reregister_php_wrapper();
  }
}

int hook_stream_wrappers() {
  TSRMLS_FETCH();

  HOOK_FUNCTION("stream_wrapper_register", sp_internal_functions_hook,
                PHP_FN(sp_stream_wrapper_register));

  HOOK_FUNCTION("stream_wrapper_restore", sp_internal_functions_hook,
                PHP_FN(sp_stream_wrapper_restore));

  sp_reregister_php_wrapper();

  return SUCCESS;
}
