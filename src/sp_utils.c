#include "php_snuffleupagus.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

static char const* const default_ipaddr = "0.0.0.0";
const char* get_ipaddr() {
  const char* client_ip = getenv("REMOTE_ADDR");
  if (client_ip) {
    return client_ip;
  }

  const char* fwd_ip = getenv("HTTP_X_FORWARDED_FOR");
  if (fwd_ip) {
    return fwd_ip;
  }

  return default_ipaddr;
}

void sp_log_msgf(char const* const restrict feature, int level, int type,
                 char const* const restrict fmt, ...) {
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

  switch (SPCFG(log_media)) {
    case SP_SYSLOG: {
      const char* error_filename = zend_get_executed_filename();
      int syslog_level = (level == E_ERROR) ? LOG_ERR : LOG_INFO;
      int error_lineno = zend_get_executed_lineno(TSRMLS_C);
      openlog(PHP_SNUFFLEUPAGUS_EXTNAME, LOG_PID, LOG_AUTH);
      syslog(syslog_level, "[snuffleupagus][%s][%s][%s] %s in %s on line %d",
             client_ip, feature, logtype, msg, error_filename, error_lineno);
      closelog();
      efree(msg);
      if (type == SP_TYPE_DROP) {
        zend_bailout();
      }
      break;
    }
    case SP_ZEND:
    default:
      zend_error(level, "[snuffleupagus][%s][%s][%s] %s", client_ip, feature,
                 logtype, msg);
      efree(msg);
      break;
  }
}

int compute_hash(char const* const restrict filename,
                 char* restrict file_hash) {
  unsigned char buf[1024] = {0};
  unsigned char digest[SHA256_SIZE] = {0};
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

int sp_log_request(zend_string const* const restrict folder, zend_string const* const restrict text_repr) {
  FILE* file;
  char const* const current_filename = zend_get_executed_filename(TSRMLS_C);
  const int current_line = zend_get_executed_lineno(TSRMLS_C);
  char filename[PATH_MAX] = {0};
  static const struct {
    char const* const str;
    const int key;
  } zones[] = {{"GET", TRACK_VARS_GET},       {"POST", TRACK_VARS_POST},
               {"COOKIE", TRACK_VARS_COOKIE}, {"SERVER", TRACK_VARS_SERVER},
               {"ENV", TRACK_VARS_ENV},       {NULL, 0}};

  PHP_SHA256_CTX context;
  unsigned char digest[SHA256_SIZE] = {0};
  char strhash[65] = {0};

  if (-1 == mkdir(ZSTR_VAL(folder), 0700) && errno != EEXIST) {
    sp_log_warn("request_logging", "Unable to create the folder '%s'",
                ZSTR_VAL(folder));
    return -1;
  }

  /* We're using the sha256 sum of the rule's textual representation, as well
   * as the stacktrace as filename, in order to only have one dump per rule, to
   * mitigate DoS attacks. We're doing the walk-the-execution-context dance
   * twice because it's easier than to cache it in a linked-list. It doesn't
   * really matter, since this is a super-cold path anyway.
   */
  PHP_SHA256Init(&context);
  PHP_SHA256Update(&context, (const unsigned char*)ZSTR_VAL(text_repr), ZSTR_LEN(text_repr));
  zend_execute_data* orig_execute_data = EG(current_execute_data);
  zend_execute_data* current = EG(current_execute_data);
  while (current) {
    EG(current_execute_data) = current;
    char* const complete_path_function = get_complete_function_path(current);
    if (complete_path_function) {
      PHP_SHA256Update(&context, (const unsigned char*)complete_path_function, strlen(complete_path_function));
      efree(complete_path_function);
    }
    current = current->prev_execute_data;
  }
  EG(current_execute_data) = orig_execute_data;
  PHP_SHA256Final(digest, &context);
  make_digest_ex(strhash, digest, SHA256_SIZE);
  snprintf(filename, PATH_MAX - 1, "%s/sp_dump.%s", ZSTR_VAL(folder), strhash);

  if (NULL == (file = fopen(filename, "w+"))) {
    sp_log_warn("request_logging", "Unable to open %s: %s", filename,
                strerror(errno));
    return -1;
  }

  fputs("RULE: ", file);
  fputs(ZSTR_VAL(text_repr), file);
  fputc('\n', file);

  fputs("FILE: ", file);
  fputs(current_filename, file);
  fprintf(file, ":%d\n", current_line);

  orig_execute_data = EG(current_execute_data);
  current = EG(current_execute_data);
  while (current) {
    EG(current_execute_data) = current;
    char* const complete_path_function = get_complete_function_path(current);
    if (complete_path_function) {
      const int current_line = zend_get_executed_lineno(TSRMLS_C);
      fputs("STACKTRACE: ", file);
      fputs(complete_path_function, file);
      fprintf(file, ":%d\n", current_line);
      efree(complete_path_function);
    }
    current = current->prev_execute_data;
  }
  EG(current_execute_data) = orig_execute_data;

  for (size_t i = 0; zones[i].str; i++) {
    zval* variable_value;
    zend_string* variable_key;

    if (Z_TYPE(PG(http_globals)[zones[i].key]) == IS_UNDEF) {
      continue;
    }

    HashTable* ht = Z_ARRVAL(PG(http_globals)[zones[i].key]);
    fputs(zones[i].str, file);
    fputc(':', file);
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, variable_key, variable_value) {
      smart_str a = {0};
      php_var_export_ex(variable_value, 1, &a);
      ZSTR_VAL(a.s)[ZSTR_LEN(a.s)] = '\0';
      fputs(ZSTR_VAL(variable_key), file);
      fputc('=', file);
      fputs(ZSTR_VAL(a.s), file);
      fputc(' ', file);
      zend_string_release(a.s);
    }
    ZEND_HASH_FOREACH_END();
    fputc('\n', file);
  }

  if (UNEXPECTED(0 != SPG(in_eval))) {
    fputs("EVAL_CODE: ", file);
#if PHP_VERSION_ID >= 80000
    fputs(ZSTR_VAL(SPG(eval_source_string)), file);
#else
    fputs(Z_STRVAL_P(SPG(eval_source_string)), file);
#endif
    fputc('\n', file);
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
      return zend_string_init(ZEND_STRL("FALSE"), 0);
    case IS_TRUE:
      return zend_string_init(ZEND_STRL("TRUE"), 0);
    case IS_NULL:
      return zend_string_init(ZEND_STRL("NULL"), 0);
    case IS_OBJECT:
      return zend_string_init(ZEND_STRL("OBJECT"), 0);
    case IS_ARRAY:
      return zend_string_init(ZEND_STRL("ARRAY"), 0);
    case IS_RESOURCE:
      return zend_string_init(ZEND_STRL("RESOURCE"), 0);
    default:                              // LCOV_EXCL_LINE
      return zend_string_init("", 0, 0);  // LCOV_EXCL_LINE
  }
}

bool sp_match_value(const zend_string* value, const zend_string* to_match, const sp_regexp* rx) {
  if (to_match) {
    return (sp_zend_string_equals(to_match, value));
  } else if (rx) {
    return sp_is_regexp_matching_zstr(rx, value);
  }
  return true;
}

void sp_log_disable(const char* restrict path, const char* restrict arg_name,
                    const zend_string* restrict arg_value,
                    const sp_disabled_function* config_node) {
  const zend_string* dump = config_node->dump;
  const zend_string* alias = config_node->alias;
  const int sim = config_node->simulation;

  if (dump) {
    sp_log_request(config_node->dump, config_node->textual_representation);
  }
  if (arg_name) {
    char* char_repr = NULL;
    if (arg_value) {
      zend_string *arg_value_dup = zend_string_init(ZSTR_VAL(arg_value), ZSTR_LEN(arg_value), 0);
      arg_value_dup = php_raw_url_encode(ZSTR_VAL(arg_value_dup), ZSTR_LEN(arg_value_dup));
      char_repr = zend_string_to_char(arg_value_dup);
      size_t max_len = MIN(ZSTR_LEN(arg_value_dup), (size_t)SPCFG(log_max_len));
      char_repr[max_len] = '\0';
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
    sp_log_request(dump, config_node->textual_representation);
  }
  if (ret_value) {
    zend_string *ret_value_dup = zend_string_init(ZSTR_VAL(ret_value), ZSTR_LEN(ret_value), 0);
    ret_value_dup = php_raw_url_encode(ZSTR_VAL(ret_value_dup), ZSTR_LEN(ret_value_dup));
    char_repr = zend_string_to_char(ret_value_dup);
    size_t max_len = MIN(ZSTR_LEN(ret_value_dup), (size_t)SPCFG(log_max_len));
    char_repr[max_len] = '\0';
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

bool sp_match_array_key(const zval* zv, const zend_string* to_match, const sp_regexp* rx) {
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

bool sp_match_array_value(const zval* arr, const zend_string* to_match, const sp_regexp* rx) {
  zval* value;

  ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), value) {
    if (Z_TYPE_P(value) != IS_ARRAY) {
      if (sp_match_value(sp_zval_to_zend_string(value), to_match, rx)) {
        return true;
      }
    } else if (sp_match_array_value(value, to_match, rx)) {
      return true;
    }
  }
  ZEND_HASH_FOREACH_END();
  return false;
}

bool /* success */ _hook_function(const char* original_name, HashTable* hook_table, zif_handler new_function) {
  zend_function* func;
  if ((func = zend_hash_str_find_ptr(CG(function_table), VAR_AND_LEN(original_name)))) {
    if (func->type != ZEND_INTERNAL_FUNCTION) {
      return false;
    }
    if (func->internal_function.handler == new_function) {
      return true;
    }
    if (zend_hash_str_add_new_ptr((hook_table), VAR_AND_LEN(original_name),
                                  func->internal_function.handler) == NULL) {
      // LCOV_EXCL_START
      sp_log_err("function_pointer_saving", "Could not save function pointer for %s", original_name);
      return false;
      // LCOV_EXCL_STOP
    }
    func->internal_function.handler = new_function;
    return true;
  }
  return false;
}

bool hook_function(const char* original_name, HashTable* hook_table, zif_handler new_function) {
  bool ret = _hook_function(original_name, hook_table, new_function);

#if PHP_VERSION_ID < 80000
  CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
#endif

  /* The `mb` module likes to hook functions, like strlen->mb_strlen,
  * so we have to hook both of them. */

  if (!CG(multibyte) && 0 == strncmp(original_name, "mb_", 3)) {
    _hook_function(original_name + 3, hook_table, new_function);
  } else if (CG(multibyte)) {
    // LCOV_EXCL_START
    char* mb_name = ecalloc(strlen(original_name) + 3 + 1, 1);
    if (NULL == mb_name) {
      return FAILURE;
    }
    memcpy(mb_name, ZEND_STRL("mb_"));
    memcpy(mb_name + 3, VAR_AND_LEN(original_name));
    _hook_function(mb_name, hook_table, new_function);
    efree(mb_name);
    // LCOV_EXCL_STOP
  }

  return ret;
}

int hook_regexp(const sp_pcre* regexp, HashTable* hook_table, zif_handler new_function) {
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

void unhook_functions(HashTable *ht) {
  zend_string *fname;
  zif_handler orig_handler;
  zend_ulong idx;

  ZEND_HASH_REVERSE_FOREACH_KEY_PTR(ht, idx, fname, orig_handler)
    zend_function *func = zend_hash_find_ptr(CG(function_table), fname);
    if (func && func->type == ZEND_INTERNAL_FUNCTION && orig_handler) {
      func->internal_function.handler = orig_handler;
    }
    (void)idx;//silence a -Wunused-but-set-variable
  ZEND_HASH_FOREACH_END();
}

bool check_is_in_eval_whitelist(const char* function_name) {
  const sp_list_node* it = SPCFG(eval).whitelist;
  if (!it) {
    return false;
  }

  /* yes, we could use a HashTable instead, but since the list is pretty
   * small, it doesn't make a difference in practise. */
  while (it && it->data) {
    if (sp_zend_string_equals_str((const zend_string*)(it->data), VAR_AND_LEN(function_name))) {
      /* We've got a match, the function is whiteslited. */
      return true;
    }
    it = it->next;
  }
  return false;
}
