#include "php_snuffleupagus.h"

bool sp_zend_string_equals(const zend_string* s1, const zend_string* s2) {
  // We can't use `zend_string_equals` here because it doesn't work on
  // `const` zend_string.
  return ZSTR_LEN(s1) == ZSTR_LEN(s2) &&
         !memcmp(ZSTR_VAL(s1), ZSTR_VAL(s2), ZSTR_LEN(s1));
}

static const char* default_ipaddr = "0.0.0.0";
const char* get_ipaddr() {
  const char* client_ip = getenv("REMOTE_ADDR");
  if (client_ip) {
    return client_ip;
  }

  const char* fwd_ip = getenv("HTTP_X_FORWARDED_FOR");
  if (fwd_ip) {
    return fwd_ip;
  }

  /* Some hosters (like heroku, see
   * https://github.com/jvoisin/snuffleupagus/issues/336) are clearing the
   * environment variables, so we don't have access to them, hence why we're
   * resorting to $_SERVER['REMOTE_ADDR'].
   */
  if (!Z_ISUNDEF(PG(http_globals)[TRACK_VARS_SERVER])) {
    const zval* const globals_client_ip =
        zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]),
                           "REMOTE_ADDR", sizeof("REMOTE_ADDR") - 1);
    if (globals_client_ip) {
      if (Z_TYPE_P(globals_client_ip) == IS_STRING) {
        if (Z_STRLEN_P(globals_client_ip) != 0) {
          return estrdup(Z_STRVAL_P(globals_client_ip));
        }
      }
    }
  }

  return default_ipaddr;
}

void sp_log_msgf(char const* restrict feature, int level, int type,
                 const char* restrict fmt, ...) {
  char* msg;
  va_list args;

  va_start(args, fmt);
  vspprintf(&msg, 0, fmt, args);
  va_end(args);

  const char* client_ip = get_ipaddr();
  const char* logtype = NULL;
  switch (type) {
    case SP_TYPE_SIMULATION:
      logtype = "simulation";
      break;
    case SP_TYPE_DROP:
      logtype = "drop";
      break;
    case SP_TYPE_LOG:
    default:
      logtype = "log";
      break;
  }

  switch (SNUFFLEUPAGUS_G(config).log_media) {
    case SP_SYSLOG: {
      const char* error_filename = zend_get_executed_filename();
      int syslog_level = (level == E_ERROR) ? LOG_ERR : LOG_INFO;
      int error_lineno = zend_get_executed_lineno(TSRMLS_C);
      openlog(PHP_SNUFFLEUPAGUS_EXTNAME, LOG_PID, LOG_AUTH);
      syslog(syslog_level, "[snuffleupagus][%s][%s][%s] %s in %s on line %d",
             client_ip, feature, logtype, msg, error_filename, error_lineno);
      closelog();
      if (type == SP_TYPE_DROP) {
        zend_bailout();
      }
      break;
    }
    case SP_ZEND:
    default:
      zend_error(level, "[snuffleupagus][%s][%s][%s] %s", client_ip, feature,
                 logtype, msg);
      break;
  }
}

int compute_hash(const char* const restrict filename,
                 char* restrict file_hash) {
  unsigned char buf[1024];
  unsigned char digest[SHA256_SIZE];
  PHP_SHA256_CTX context;
  size_t n;

  php_stream* stream =
      php_stream_open_wrapper(filename, "rb", REPORT_ERRORS, NULL);
  if (!stream) {
    // LCOV_EXCL_START
    sp_log_err("hash_computation",
               "Can not open the file %s to compute its hash", filename);
    return FAILURE;
    // LCOV_EXCL_STOP
  }

  PHP_SHA256Init(&context);
  while ((n = php_stream_read(stream, (char*)buf, sizeof(buf))) > 0) {
    PHP_SHA256Update(&context, buf, n);
  }
  PHP_SHA256Final(digest, &context);
  php_stream_close(stream);
  make_digest_ex(file_hash, digest, SHA256_SIZE);
  return SUCCESS;
}

static int construct_filename(char* filename,
                              const zend_string* restrict folder,
                              const zend_string* restrict textual) {
  PHP_SHA256_CTX context;
  unsigned char digest[SHA256_SIZE] = {0};
  char strhash[65] = {0};

  if (-1 == mkdir(ZSTR_VAL(folder), 0700) && errno != EEXIST) {
    sp_log_warn("request_logging", "Unable to create the folder '%s'",
                ZSTR_VAL(folder));
    return -1;
  }

  /* We're using the sha256 sum of the rule's textual representation
   * as filename, in order to only have one dump per rule, to migitate
   * DoS attacks. */
  PHP_SHA256Init(&context);
  PHP_SHA256Update(&context, (const unsigned char*)ZSTR_VAL(textual),
                   ZSTR_LEN(textual));
  PHP_SHA256Final(digest, &context);
  make_digest_ex(strhash, digest, SHA256_SIZE);
  snprintf(filename, PATH_MAX - 1, "%s/sp_dump.%s", ZSTR_VAL(folder), strhash);

  return 0;
}

int sp_log_request(const zend_string* restrict folder,
                   const zend_string* restrict text_repr, char* from) {
  FILE* file;
  const char* current_filename = zend_get_executed_filename(TSRMLS_C);
  const int current_line = zend_get_executed_lineno(TSRMLS_C);
  char filename[PATH_MAX] = {0};
  const struct {
    char const* const str;
    const int key;
  } zones[] = {{"GET", TRACK_VARS_GET},       {"POST", TRACK_VARS_POST},
               {"COOKIE", TRACK_VARS_COOKIE}, {"SERVER", TRACK_VARS_SERVER},
               {"ENV", TRACK_VARS_ENV},       {NULL, 0}};

  if (0 != construct_filename(filename, folder, text_repr)) {
    return -1;
  }
  if (NULL == (file = fopen(filename, "w+"))) {
    sp_log_warn("request_logging", "Unable to open %s: %s", filename,
                strerror(errno));
    return -1;
  }

  fprintf(file, "RULE: sp%s%s\n", from, ZSTR_VAL(text_repr));

  fprintf(file, "FILE: %s:%d\n", current_filename, current_line);
  for (size_t i = 0; i < (sizeof(zones) / sizeof(zones[0])) - 1; i++) {
    zval* variable_value;
    zend_string* variable_key;

    if (Z_TYPE(PG(http_globals)[zones[i].key]) == IS_UNDEF) {
      continue;
    }

    HashTable* ht = Z_ARRVAL(PG(http_globals)[zones[i].key]);
    fprintf(file, "%s:", zones[i].str);
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, variable_key, variable_value) {
      smart_str a;

      memset(&a, 0, sizeof(a));
      php_var_export_ex(variable_value, 1, &a);
      ZSTR_VAL(a.s)[ZSTR_LEN(a.s)] = '\0';
      fprintf(file, "%s=%s ", ZSTR_VAL(variable_key), ZSTR_VAL(a.s));
      zend_string_release(a.s);
    }
    ZEND_HASH_FOREACH_END();
    fputs("\n", file);
  }
  fclose(file);

  return 0;
}

static char* zend_string_to_char(const zend_string* zs) {
  // Remove all \0 in a zend_string and replace them with '0' instead.

  if (ZSTR_LEN(zs) + 1 < ZSTR_LEN(zs)) {
    // LCOV_EXCL_START
    sp_log_err("overflow_error",
               "Overflow tentative detected in zend_string_to_char");
    zend_bailout();
    // LCOV_EXCL_STOP
  }

  char* copy = ecalloc(ZSTR_LEN(zs) + 1, 1);
  for (size_t i = 0; i < ZSTR_LEN(zs); i++) {
    if (ZSTR_VAL(zs)[i] == '\0') {
      copy[i] = '0';
    } else {
      copy[i] = ZSTR_VAL(zs)[i];
    }
  }
  return copy;
}

const zend_string* sp_zval_to_zend_string(const zval* zv) {
  switch (Z_TYPE_P(zv)) {
    case IS_LONG: {
      char* msg;
      spprintf(&msg, 0, ZEND_LONG_FMT, Z_LVAL_P(zv));
      zend_string* zs = zend_string_init(msg, strlen(msg), 0);
      efree(msg);
      return zs;
    }
    case IS_DOUBLE: {
      char* msg;
      spprintf(&msg, 0, "%f", Z_DVAL_P(zv));
      zend_string* zs = zend_string_init(msg, strlen(msg), 0);
      efree(msg);
      return zs;
    }
    case IS_STRING: {
      return Z_STR_P(zv);
    }
    case IS_FALSE:
      return zend_string_init("FALSE", sizeof("FALSE") - 1, 0);
    case IS_TRUE:
      return zend_string_init("TRUE", sizeof("TRUE") - 1, 0);
    case IS_NULL:
      return zend_string_init("NULL", sizeof("NULL") - 1, 0);
    case IS_OBJECT:
      return zend_string_init("OBJECT", sizeof("OBJECT") - 1, 0);
    case IS_ARRAY:
      return zend_string_init("ARRAY", sizeof("ARRAY") - 1, 0);
    case IS_RESOURCE:
      return zend_string_init("RESOURCE", sizeof("RESOURCE") - 1, 0);
    default:                              // LCOV_EXCL_LINE
      return zend_string_init("", 0, 0);  // LCOV_EXCL_LINE
  }
}

bool sp_match_value(const zend_string* value, const zend_string* to_match,
                    const sp_pcre* rx) {
  if (to_match) {
    return (sp_zend_string_equals(to_match, value));
  } else if (rx) {
    char* tmp = zend_string_to_char(value);
    bool ret = sp_is_regexp_matching(rx, tmp);
    efree(tmp);
    return ret;
  } else {
    return true;
  }
  return false;
}

void sp_log_disable(const char* restrict path, const char* restrict arg_name,
                    const zend_string* restrict arg_value,
                    const sp_disabled_function* config_node) {
  const zend_string* dump = config_node->dump;
  const zend_string* alias = config_node->alias;
  const int sim = config_node->simulation;

  if (dump) {
    sp_log_request(config_node->dump, config_node->textual_representation,
                   SP_TOKEN_DISABLE_FUNC);
  }
  if (arg_name) {
    char* char_repr = NULL;
    if (arg_value) {
      char_repr = zend_string_to_char(arg_value);
    }
    if (alias) {
      sp_log_auto(
          "disabled_function", sim,
          "Aborted execution on call of the function '%s', "
          "because its argument '%s' content (%s) matched the rule '%s'",
          path, arg_name, char_repr ? char_repr : "?", ZSTR_VAL(alias));
    } else {
      sp_log_auto("disabled_function", sim,
                  "Aborted execution on call of the function '%s', "
                  "because its argument '%s' content (%s) matched a rule",
                  path, arg_name, char_repr ? char_repr : "?");
    }
    efree(char_repr);
  } else {
    if (alias) {
      sp_log_auto("disabled_function", sim,
                  "Aborted execution on call of the function '%s', "
                  "because of the the rule '%s'",
                  path, ZSTR_VAL(alias));
    } else {
      sp_log_auto("disabled_function", sim,
                  "Aborted execution on call of the function '%s'", path);
    }
  }
}

void sp_log_disable_ret(const char* restrict path,
                        const zend_string* restrict ret_value,
                        const sp_disabled_function* config_node) {
  const zend_string* dump = config_node->dump;
  const zend_string* alias = config_node->alias;
  const int sim = config_node->simulation;
  char* char_repr = NULL;

  if (dump) {
    sp_log_request(dump, config_node->textual_representation,
                   SP_TOKEN_DISABLE_FUNC);
  }
  if (ret_value) {
    char_repr = zend_string_to_char(ret_value);
  }
  if (alias) {
    sp_log_auto(
        "disabled_function", sim,
        "Aborted execution on return of the function '%s', "
        "because the function returned '%s', which matched the rule '%s'",
        path, char_repr ? char_repr : "?", ZSTR_VAL(alias));
  } else {
    sp_log_auto("disabled_function", sim,
                "Aborted execution on return of the function '%s', "
                "because the function returned '%s', which matched a rule",
                path, char_repr ? char_repr : "?");
  }
  efree(char_repr);
}

bool sp_match_array_key(const zval* zv, const zend_string* to_match,
                        const sp_pcre* rx) {
  zend_string* key;
  zend_ulong idx;

  ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(zv), idx, key) {
    if (key) {
      if (sp_match_value(key, to_match, rx)) {
        return true;
      }
    } else {
      char* idx_str = NULL;
      spprintf(&idx_str, 0, ZEND_ULONG_FMT, idx);
      zend_string* tmp = zend_string_init(idx_str, strlen(idx_str), 0);
      if (sp_match_value(tmp, to_match, rx)) {
        efree(idx_str);
        return true;
      }
      efree(idx_str);
    }
  }
  ZEND_HASH_FOREACH_END();
  return false;
}

bool sp_match_array_value(const zval* arr, const zend_string* to_match,
                          const sp_pcre* rx) {
  zval* value;

  ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), value) {
    if (Z_TYPE_P(value) != IS_ARRAY) {
      const zend_string* value_str = sp_zval_to_zend_string(value);
      if (sp_match_value(value_str, to_match, rx)) {
        return true;
      } else {
      }
    } else if (sp_match_array_value(value, to_match, rx)) {
      return true;
    }
  }
  ZEND_HASH_FOREACH_END();
  return false;
}

int hook_function(const char* original_name, HashTable* hook_table,
                  zif_handler new_function) {
  zend_internal_function* func;
  bool ret = FAILURE;

  /* The `mb` module likes to hook functions, like strlen->mb_strlen,
   * so we have to hook both of them. */

  if ((func = zend_hash_str_find_ptr(CG(function_table),
                                     VAR_AND_LEN(original_name)))) {
    if (func->handler == new_function) {
      return SUCCESS;  // the function is already hooked
    } else {
      if (zend_hash_str_add_new_ptr((hook_table), VAR_AND_LEN(original_name),
                                    func->handler) == NULL) {
        // LCOV_EXCL_START
        sp_log_err("function_pointer_saving",
                   "Could not save function pointer for %s", original_name);
        return FAILURE;
        // LCOV_EXCL_STOP
      }
      func->handler = new_function;
      ret = SUCCESS;
    }
  }

#if PHP_VERSION_ID < 80000
  CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
#endif

  if (0 == strncmp(original_name, "mb_", 3) && !CG(multibyte)) {
    if (zend_hash_str_find(CG(function_table),
                           VAR_AND_LEN(original_name + 3))) {
      return hook_function(original_name + 3, hook_table, new_function);
    }
  } else if (CG(multibyte)) {
    // LCOV_EXCL_START
    char* mb_name = ecalloc(strlen(original_name) + 3 + 1, 1);
    if (NULL == mb_name) {
      return FAILURE;
    }
    memcpy(mb_name, "mb_", sizeof("mb_") - 1);
    memcpy(mb_name + 3, VAR_AND_LEN(original_name));
    if (zend_hash_str_find(CG(function_table), VAR_AND_LEN(mb_name))) {
      return hook_function(mb_name, hook_table, new_function);
    }
    free(mb_name);
    // LCOV_EXCL_STOP
  }

  return ret;
}

int hook_regexp(const sp_pcre* regexp, HashTable* hook_table,
                zif_handler new_function) {
  zend_string* key;

  ZEND_HASH_FOREACH_STR_KEY(CG(function_table), key)
  if (key) {
    if (true == sp_is_regexp_matching_len(regexp, key->val, key->len)) {
      hook_function(key->val, hook_table, new_function);
    }
  }
  ZEND_HASH_FOREACH_END();

  return SUCCESS;
}

bool check_is_in_eval_whitelist(const zend_string* const function_name) {
  const sp_list_node* it = SNUFFLEUPAGUS_G(config).config_eval->whitelist;

  if (!it) {
    return false;
  }

  /* yes, we could use a HashTable instead, but since the list is pretty
   * small, it doesn't make a difference in practise. */
  while (it && it->data) {
    if (sp_zend_string_equals(function_name, (const zend_string*)(it->data))) {
      /* We've got a match, the function is whiteslited. */
      return true;
    }
    it = it->next;
  }
  return false;
}
