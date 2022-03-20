#include "php_snuffleupagus.h"

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
    orig_handler = zend_hash_str_find_ptr(SPG(sp_internal_functions_hook), ZEND_STRL("stream_wrapper_register"));
    orig_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  }
}

int hook_stream_wrappers() {
  TSRMLS_FETCH();

  HOOK_FUNCTION("stream_wrapper_register", sp_internal_functions_hook,
                PHP_FN(sp_stream_wrapper_register));

  return SUCCESS;
}
