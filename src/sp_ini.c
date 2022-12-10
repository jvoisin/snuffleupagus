#include "php_snuffleupagus.h"

#define SP_INI_HAS_CHECKS_COND(entry) (entry->min || entry->max || entry->regexp)
#define SP_INI_ACCESS_READONLY_COND(entry, cfg) (entry->access == SP_READONLY || (!entry->access && cfg->policy_readonly))

#define sp_log_auto2(feature, is_simulation, drop, ...) \
  sp_log_msgf(feature, ((is_simulation || !drop) ? SP_LOG_WARN : SP_LOG_ERROR), \
            (is_simulation ? SP_TYPE_SIMULATION : (drop ? SP_TYPE_DROP : SP_TYPE_LOG)),   \
            __VA_ARGS__)
#define sp_log_ini_check_violation(...) if (simulation || cfg->policy_drop || (entry && entry->drop) || !cfg->policy_silent_fail) { \
    sp_log_auto2("ini_protection", simulation, (cfg->policy_drop || (entry && entry->drop)), __VA_ARGS__); \
  }


static bool /* success */ sp_ini_check(zend_string *const restrict varname, zend_string const *const restrict new_value, sp_ini_entry **sp_entry_p) {
  if (!varname || ZSTR_LEN(varname) == 0) {
    return false;
  }

  sp_config_ini const* const cfg = &(SPCFG(ini));
  sp_ini_entry *entry = zend_hash_find_ptr(cfg->entries, varname);
  if (sp_entry_p) {
    *sp_entry_p = entry;
  }
  bool simulation = (cfg->simulation || (entry && entry->simulation));

  if (!entry) {
    if (cfg->policy_readonly) {
      if (!cfg->policy_silent_ro) {
        sp_log_ini_check_violation("INI setting is read-only");
      }
      return simulation;
    }
    return true;
  }

  // we have an entry.

  if (SP_INI_ACCESS_READONLY_COND(entry, cfg)) {
    if (!cfg->policy_silent_ro) {
      sp_log_ini_check_violation("%s", (entry->msg ? ZSTR_VAL(entry->msg) : "INI setting is read-only"));
    }
    return simulation;
  }

  if (!new_value || ZSTR_LEN(new_value) == 0) {
    if (entry->allow_null) {
      return true; // allow NULL value and skip other tests
    }
    if (SP_INI_HAS_CHECKS_COND(entry)) {
      sp_log_ini_check_violation("new INI value must not be NULL or empty");
      return simulation;
    }
    return true; // no new_value, but no checks to perform
  }

  // we have a new_value.

  if (entry->min || entry->max) {
#if PHP_VERSION_ID >= 80200
    zend_long lvalue = ZEND_STRTOL(ZSTR_VAL(new_value), NULL, 0);
    if ((entry->min && ZEND_STRTOL(ZSTR_VAL(entry->min), NULL, 0) > lvalue) ||
        (entry->max && ZEND_STRTOL(ZSTR_VAL(entry->max), NULL, 0) < lvalue)) {
#else
    zend_long lvalue = zend_atol(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
    if ((entry->min && zend_atol(ZSTR_VAL(entry->min), ZSTR_LEN(entry->min)) > lvalue) ||
        (entry->max && zend_atol(ZSTR_VAL(entry->max), ZSTR_LEN(entry->max)) < lvalue)) {
#endif
      sp_log_ini_check_violation("%s", (entry->msg ? ZSTR_VAL(entry->msg) : "INI value out of range"));
      return simulation;
    }
  }

  if (entry->regexp) {
    if (!sp_is_regexp_matching_zstr(entry->regexp, new_value)) {
      sp_log_ini_check_violation("%s", (entry->msg ? ZSTR_VAL(entry->msg) : "INI value does not match regex"));
      return simulation;
    }
  }

  return true;
}

static PHP_INI_MH(sp_ini_onmodify) {
  sp_ini_entry *sp_entry = NULL;

  if (!sp_ini_check(entry->name, new_value, &sp_entry)) {
    return FAILURE;
  }

  if (sp_entry && sp_entry->orig_onmodify) {
    return sp_entry->orig_onmodify(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
  }

  return SUCCESS;
}

void sp_hook_ini() {
  sp_config_ini const* const cfg = &(SPCFG(ini));
  sp_ini_entry *sp_entry;
  zend_ini_entry *ini_entry;
  ZEND_HASH_FOREACH_PTR(cfg->entries, sp_entry)
    if ((ini_entry = zend_hash_find_ptr(EG(ini_directives), sp_entry->key)) == NULL) {
      sp_log_warn("ini_protection", "Cannot hook INI var `%s`. Maybe a typo or the PHP extension providing this var is not loaded yet.", ZSTR_VAL(sp_entry->key));
      continue;
    }
    if (SP_INI_ACCESS_READONLY_COND(sp_entry, cfg) && (cfg->policy_silent_ro || cfg->policy_silent_fail) && !sp_entry->drop && !(sp_entry->simulation || cfg->simulation)) {
      ini_entry->modifiable = ini_entry->orig_modifiable = 0;
    }
    PHP_INI_MH((*orig_onmodify)) = ini_entry->on_modify;

    if (SP_INI_HAS_CHECKS_COND(sp_entry) || SP_INI_ACCESS_READONLY_COND(sp_entry, cfg)) {
      // only hook on_modify if there is any check to perform
      sp_entry->orig_onmodify = ini_entry->on_modify;
      ini_entry->on_modify = sp_ini_onmodify;
    }

    if (sp_entry->set) {
      zend_string *duplicate = zend_string_copy(sp_entry->set);

      if (!orig_onmodify || orig_onmodify(ini_entry, duplicate, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, ZEND_INI_STAGE_STARTUP) == SUCCESS) {
        ini_entry->value = duplicate;
      } else {
        zend_string_release(duplicate);
        sp_log_warn("ini_protection", "Failed to set INI var `%s`.", ZSTR_VAL(sp_entry->key));
        continue;
      }
    }
  ZEND_HASH_FOREACH_END();
}

void sp_unhook_ini() {
  sp_ini_entry *sp_entry;
  ZEND_HASH_FOREACH_PTR(SPCFG(ini).entries, sp_entry)
    zend_ini_entry *ini_entry;
    if (!sp_entry->orig_onmodify) {
      // not hooked or no original onmodify
      continue;
    }
    if ((ini_entry = zend_hash_find_ptr(EG(ini_directives), sp_entry->key)) == NULL) {
      // unusual. ini entry is missing.
      continue;
    }
    ini_entry->on_modify = sp_entry->orig_onmodify;
    sp_entry->orig_onmodify = NULL;
  ZEND_HASH_FOREACH_END();
}
