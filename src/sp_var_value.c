#include "php_snuffleupagus.h"

static zval *get_param_var(const zend_execute_data *const ed,
                           const char *const var_name, bool print) {
  unsigned int nb_param = ed->func->common.num_args;

  for (unsigned int i = 0; i < nb_param; i++) {
    const char *arg_name;
    if (ZEND_USER_CODE(ed->func->type)) {
      arg_name = ZSTR_VAL(ed->func->common.arg_info[i].name);
    } else {
      arg_name = ed->func->internal_function.arg_info[i].name;
    }
    if (0 == strcmp(arg_name, var_name)) {
      return ZEND_CALL_VAR_NUM(ed, i);
    }
    if (print == true) {
      sp_log_warn("config", "  - %d parameter's name: '%s'", i, arg_name);
    }
  }
  return NULL;
}

static zval *get_local_var(zend_execute_data *ed, const char *const var_name) {
  zend_execute_data *orig_execute_data = ed;
  zend_execute_data *current = ed;

  while (current) {
    zval *value = NULL;
    zend_string *key = NULL;
    EG(current_execute_data) = current;
    zend_array *symtable = zend_rebuild_symbol_table();

    if (UNEXPECTED(symtable == NULL)) {
      EG(current_execute_data) = orig_execute_data;
      return NULL;
    }

    ZEND_HASH_FOREACH_STR_KEY_VAL(symtable, key, value) {
      if (0 == strcmp(var_name, key->val)) {
        if (Z_TYPE_P(value) == IS_INDIRECT) {
          value = Z_INDIRECT_P(value);
        }
        EG(current_execute_data) = orig_execute_data;
        return value;
      }
    }
    ZEND_HASH_FOREACH_END();
    current = current->prev_execute_data;
  }
  EG(current_execute_data) = orig_execute_data;
  return NULL;
}

static zval *get_constant(const char *const value) {
  zend_string *name = zend_string_init(value, strlen(value), 0);
  zval *zvalue = zend_get_constant_ex(name, NULL, ZEND_FETCH_CLASS_SILENT);
  zend_string_release(name);
  return zvalue;
}

static zval *get_var_value(zend_execute_data *ed, const char *var_name,
                           bool is_param) {
  if (!var_name) {
    return NULL;  // LCOV_EXCL_LINE
  }

  if (*var_name != VARIABLE_TOKEN) {
    return get_constant(var_name);
  } else {
    var_name++;
  }

  if (is_param) {
    zval *zvalue = get_param_var(ed, var_name, false);
    if (!zvalue) {
      char *complete_function_path = get_complete_function_path(ed);
      sp_log_warn("config",
                  "It seems that you are filtering on a parameter "
                  "'%s' of the function '%s', but the parameter does "
                  "not exists.",
                  var_name, complete_function_path);
      get_param_var(ed, var_name, true);
      efree(complete_function_path);
      return NULL;
    }
    return zvalue;
  } else {
    return get_local_var(ed, var_name);
  }
}

static void *get_entry_hashtable(const HashTable *const ht,
                                 const char *const entry, size_t entry_len) {
  zval *zvalue = zend_hash_str_find(ht, entry, entry_len);

  if (!zvalue) {
    zvalue = zend_hash_index_find(ht, atol(entry));
  }

  while (zvalue &&
         (Z_TYPE_P(zvalue) == IS_INDIRECT || Z_TYPE_P(zvalue) == IS_PTR)) {
    if (Z_TYPE_P(zvalue) == IS_INDIRECT) {
      zvalue = Z_INDIRECT_P(zvalue);
    } else {
      zvalue = Z_PTR_P(zvalue);
    }
  }
  return zvalue;
}

static zval *get_array_value(zend_execute_data *ed, const zval *const zvalue,
                             const sp_tree *const tree) {
  const zval *const idx_value = sp_get_var_value(ed, tree->idx, false);

  if (!zvalue || !idx_value) {
    return NULL;
  }

  if (Z_TYPE_P(zvalue) == IS_ARRAY) {
    const zend_string *const idx = sp_zval_to_zend_string(idx_value);
    return get_entry_hashtable(Z_ARRVAL_P(zvalue), ZSTR_VAL(idx),
                               ZSTR_LEN(idx));
  }

  return NULL;
}

static zval *get_object_property(zend_execute_data *ed, zval *object,
                                 const char *property, bool is_param) {
  const char *const class_name = object->value.obj->ce->name->val;
  const HashTable *const array = Z_OBJPROP_P(object);
  const zval *const property_val = get_var_value(ed, property, is_param);
  size_t len;

  if (property_val) {
    if (Z_TYPE_P(property_val) != IS_STRING) {
      return NULL;
    } else {
      property = Z_STRVAL_P(property_val);
    }
  }
  zval *zvalue = get_entry_hashtable(array, property, strlen(property));
  // TODO do we want to log overflow?
  if (!zvalue) {
    len = strlen(property) + 4;
    char *protected_property = emalloc(len);
    snprintf(protected_property, len, PROTECTED_PROP_FMT, 0, 0, property);
    zvalue = get_entry_hashtable(array, protected_property, len - 1);
    efree(protected_property);
  }
  if (!zvalue) {
    len = strlen(class_name) + 3 + strlen(property);
    char *private_property = emalloc(len);
    snprintf(private_property, len, PRIVATE_PROP_FMT, 0, class_name, 0,
             property);
    zvalue = get_entry_hashtable(array, private_property, len - 1);
    efree(private_property);
  }
  return zvalue;
}

static zend_class_entry *get_class(const char *const value) {
  zend_string *name = zend_string_init(value, strlen(value), 0);
  zend_class_entry *ce = zend_lookup_class(name);
  zend_string_release(name);
  return ce;
}

static zval *get_unknown_type(const char *restrict value, zval *zvalue,
                              zend_class_entry *ce, zend_execute_data *ed,
                              const sp_tree *const tree, bool is_param) {
  if (ce) {
    zvalue = get_entry_hashtable(&ce->constants_table, value, strlen(value));
  } else if (zvalue && Z_TYPE_P(zvalue) == IS_OBJECT) {
    zvalue = get_object_property(ed, zvalue, value, is_param);
  } else if (!zvalue) {
    if (tree->type == CONSTANT) {
      zvalue = get_constant(value);
    }
    if (!zvalue) {
      zvalue = emalloc(sizeof(zval));
      ZVAL_PSTRING(zvalue, value);
    }
  } else {
    return NULL;
  }
  return zvalue;
}

zval *sp_get_var_value(zend_execute_data *ed, const sp_tree *tree,
                       bool is_param) {
  zval *zvalue = NULL;
  zend_class_entry *ce = NULL;

  while (tree) {
    switch (tree->type) {
      case ARRAY:
        if (ce) {
          zvalue = get_entry_hashtable(&ce->constants_table, tree->value,
                                       strlen(tree->value));
          ce = NULL;
        } else if (!zvalue) {
          zvalue = get_var_value(ed, tree->value, is_param);
        } else if (Z_TYPE_P(zvalue) == IS_OBJECT) {
          zvalue = get_object_property(ed, zvalue, tree->value, is_param);
        }
        zvalue = get_array_value(ed, zvalue, tree);
        break;
      case VAR:
        if (zvalue && Z_TYPE_P(zvalue) == IS_OBJECT) {
          zvalue = get_object_property(ed, zvalue, tree->value, is_param);
        } else {
          zvalue = get_var_value(ed, tree->value, is_param);
        }
        break;
      case OBJECT:
        if (!zvalue) {
          zvalue = get_var_value(ed, tree->value, is_param);
        } else if (Z_TYPE_P(zvalue) == IS_OBJECT) {
          if (0 != strlen(tree->value)) {
            zvalue = get_object_property(ed, zvalue, tree->value, is_param);
          }
        } else {
          return NULL;
        }
        break;
      case CLASS:
        ce = get_class(tree->value);
        zvalue = NULL;
        break;
      default:
        zvalue = get_unknown_type(tree->value, zvalue, ce, ed, tree, is_param);
        ce = NULL;
        break;
    }
    if (!zvalue && !ce) {
      return NULL;
    }
    tree = tree->next;
  }
  return zvalue;
}
