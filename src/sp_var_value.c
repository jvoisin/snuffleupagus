#include "php_snuffleupagus.h"

static zval *get_local_var(zend_execute_data *ed, const char *var_name) {
  zend_execute_data* orig_execute_data = ed;
  zend_execute_data* current = ed;
  zval* value = NULL;
  while (current) {
    zend_string* key = NULL;
    EG(current_execute_data) = current;
    zend_array* symtable = zend_rebuild_symbol_table();
    ZEND_HASH_FOREACH_STR_KEY_VAL(symtable, key, value) {
      if (0 == strcmp(var_name, key->val)) {
	if (Z_TYPE_P(value) == IS_INDIRECT) {
	  value = Z_INDIRECT_P(value);
	}
	EG(current_execute_data) = orig_execute_data;
	return (value);
      }
    }
    ZEND_HASH_FOREACH_END();
    current = current->prev_execute_data;
  }
  EG(current_execute_data) = orig_execute_data;
  return NULL;
}

/* If the entry is numeric it is casted to a long. */
static void *get_entry_hashtable(HashTable *ht, const char *entry, int entry_len) {
  zval *zvalue = zend_hash_str_find(ht, entry, entry_len);

  if (!zvalue) {
    zvalue = zend_hash_index_find(ht, atol(entry));
  }
  while (zvalue && (Z_TYPE_P(zvalue) == IS_INDIRECT || Z_TYPE_P(zvalue) == IS_PTR)) {
    if (Z_TYPE_P(zvalue) == IS_INDIRECT) {
      zvalue = Z_INDIRECT_P(zvalue);
    } else {
      zvalue = Z_PTR_P(zvalue);
    }
  }
  return zvalue;
}

static zval *get_array_value(zend_execute_data *ed, zval *prev,
		      const arbre_du_ghetto *sapin, const char *value) {
  char *idx_value;
  zval *zvalue = NULL;

  if (!prev) {
    zvalue = get_local_var(ed, value);
  } else {
    zvalue = prev;
  }
  idx_value = get_value(ed, sapin->idx);
  if (!zvalue || !idx_value) {
    return NULL;
  }
  if (Z_TYPE_P(zvalue) == IS_ARRAY) {
    return get_entry_hashtable(Z_ARRVAL_P(zvalue), idx_value, strlen(idx_value));
  }
  return NULL;
}

static zval *get_object_property(zval *object, const char *property) {
  char *class_name = object->value.obj->ce->name->val;
  HashTable *array = Z_OBJPROP_P(object);
  zval *zvalue = NULL;
  int len;

  zvalue = get_entry_hashtable(array, property, strlen(property));
  /* Problem with heritage */
  if (!zvalue) {
    char *protected_property = malloc(sizeof(char) * (strlen(property) + 4));
    len = sprintf(protected_property, PROTECTED_PROP_FMT, 0, 0, property);
    zvalue = get_entry_hashtable(array, protected_property, len);
    free(protected_property);
  }
  if (!zvalue) {
    char *private_property = malloc(sizeof(char) * (strlen(class_name) + 3
						    + strlen(property)));
    len = sprintf(private_property, PRIVATE_PROP_FMT, 0, class_name, 0, property);
    zvalue = get_entry_hashtable(array, private_property, len);
    free(private_property);
  }
  return zvalue;
}

char *get_value(zend_execute_data *ed, const arbre_du_ghetto *sapin) {
  char *value = NULL, *ret_value = NULL;
  zval *zvalue = NULL;
  HashTable *prev_array = NULL;
  zend_string *name = NULL;
  zend_class_entry *ce = NULL;
  /*TODO: remove debug. */
  char *debug = malloc(1000);

  debug[0] = 0;
  while (sapin) {
    value = sapin->value;
    switch (sapin->type) {
      case array:
	if (prev_array) {
	  zvalue = get_entry_hashtable(prev_array, value, strlen(value));
	  prev_array = NULL;
	}
	sprintf(debug, "%s%s[%s]", strdup(debug), value, sapin->idx->value);
	zvalue = get_array_value(ed, zvalue, sapin, value);
	break;
      case var:
	sprintf(debug, "%s%s", strdup(debug), value);
	zvalue = get_local_var(ed, value);
	break;
      case object:
	if (prev_array) {
	  zvalue = get_entry_hashtable(prev_array, value, strlen(value));
	  prev_array = NULL;
	}
	if (!zvalue) {
	  sprintf(debug, "%s%s->", strdup(debug), value);
	  zvalue = get_local_var(ed, value);
	} else if (Z_TYPE_P(zvalue) == IS_OBJECT) {
	  if (0 != strlen(value)) {
	    sprintf(debug, "%s%s", strdup(debug), value);
	    goto object_zval;
	  }
	} else {
	  return NULL;
	}
	break;
      case class:
	sprintf(debug, "%s%s::", strdup(debug), value);
	name = zend_string_init(value, strlen(value), 0);
	ce = zend_lookup_class(name);
	zend_string_release(name);
	zvalue = NULL;
	if (ce) {
	  prev_array = &ce->constants_table;
	}
	break;
      default:
	sprintf(debug, "%s%s", strdup(debug), value);
	if (prev_array) {
	  zvalue = get_entry_hashtable(prev_array, value, strlen(value));
	  prev_array = NULL;
	} else if (zvalue && Z_TYPE_P(zvalue) == IS_OBJECT && 0 != strlen(value)) {
object_zval:
	  zvalue = get_object_property(zvalue, value);
	} else if (!sapin->next && !zvalue) {
	  ret_value = strdup(value);
	} else {
	  return NULL;
	}
	break;
    }
    if (!zvalue && !ret_value && !prev_array) {
      return NULL;
    }
    sapin = sapin->next;
    }
    if (zvalue) {
      ret_value = sp_convert_to_string(zvalue);
    }
    /*printf("value: %s = \"%s\"\n", debug, ret_value);*/
    free(debug);
    return ret_value;
  }
