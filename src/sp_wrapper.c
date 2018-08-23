#include "php_snuffleupagus.h"
#include "sp_config.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

void sp_disable_wrapper() {
  HashTable *orig = php_stream_get_url_stream_wrappers_hash();
  HashTable *full = pemalloc(sizeof(*full), 1);
  zval *zv;
  zend_string *zs;

  zend_hash_init(full, zend_hash_num_elements(orig), NULL, NULL, 1);
  zend_hash_copy(full, orig, NULL);
  zend_hash_clean(orig);

  ZEND_HASH_FOREACH_STR_KEY_VAL(full, zs, zv) {
    sp_list_node *tmp = SNUFFLEUPAGUS_G(config).config_wrapper->whitelist;
    while (tmp) {
      if (zend_string_equals_ci(zs, (zend_string*)tmp->data)) {
        zend_hash_add(orig, zs, zv);
        break;
      }
      tmp = tmp->next;
    }
  }

  ZEND_HASH_FOREACH_END();
  zend_hash_destroy(full);
  pefree(full, 1);
  SNUFFLEUPAGUS_G(config).config_wrapper->num_wrapper = zend_hash_num_elements(orig);
}
