#include "php_snuffleupagus.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static inline void _sp_log_err(const char* fmt, ...) {
  char* msg;
  va_list args;

  va_start(args, fmt);
  vspprintf(&msg, 0, fmt, args);
  va_end(args);
  php_log_err(msg);
}

void sp_log_msg(char const* feature, char const* level, const char* fmt, ...) {
  char* msg;
  va_list args;

  va_start(args, fmt);
  vspprintf(&msg, 0, fmt, args);
  va_end(args);

  char const* const client_ip = getenv("REMOTE_ADDR");
  _sp_log_err("[snuffleupagus][%s][%s][%s] %s",
              client_ip ? client_ip : "0.0.0.0", feature, level, msg);
}

zend_always_inline int is_regexp_matching(const pcre* regexp, const char* str) {
  int vec[30];
  int ret = 0;

  assert(NULL != regexp);
  assert(NULL != str);

  ret = sp_pcre_exec(regexp, NULL, str, strlen(str), 0, 0, vec,
                     sizeof(vec) / sizeof(int));

  if (ret < 0) {
    if (ret != PCRE_ERROR_NOMATCH) {
      sp_log_err("regexp", "Something went wrong with a regexp (%d).", ret);
    }
    return false;
  }
  return true;
}

int compute_hash(const char* const filename, char* file_hash) {
  unsigned char buf[1024];
  unsigned char digest[SHA256_SIZE];
  PHP_SHA256_CTX context;
  size_t n;

  php_stream* stream =
      php_stream_open_wrapper(filename, "rb", REPORT_ERRORS, NULL);
  if (!stream) {
    sp_log_err("hash_computation",
               "Can not open the file %s to compute its hash.\n", filename);
    return FAILURE;
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

static int construct_filename(char* filename, const char* folder,
                              const char* textual) {
  PHP_SHA256_CTX context;
  unsigned char digest[SHA256_SIZE] = {0};
  char strhash[65] = {0};

  if (-1 == mkdir(folder, 0700) && errno != EEXIST) {
    sp_log_err("request_logging", "Unable to create the folder '%s'.", folder);
    return -1;
  }

  /* We're using the sha256 sum of the rule's textual representation
   * as filename, in order to only have one dump per rule, to migitate
   * DoS attacks. */
  PHP_SHA256Init(&context);
  PHP_SHA256Update(&context, (const unsigned char*)textual, strlen(textual));
  PHP_SHA256Final(digest, &context);
  make_digest_ex(strhash, digest, SHA256_SIZE);
  snprintf(filename, PATH_MAX - 1, "%s/sp_dump.%s", folder, strhash);

  return 0;
}

int sp_log_request(const char* folder, const char* text_repr) {
  FILE* file;
  const char* current_filename = zend_get_executed_filename(TSRMLS_C);
  const int current_line = zend_get_executed_lineno(TSRMLS_C);
  char filename[PATH_MAX] = {0};
  const struct {
    const char* str;
    const int key;
  } zones[] = {{"GET", TRACK_VARS_GET},       {"POST", TRACK_VARS_POST},
               {"COOKIE", TRACK_VARS_COOKIE}, {"SERVER", TRACK_VARS_SERVER},
               {"ENV", TRACK_VARS_ENV},       {NULL, 0}};

  if (0 != construct_filename(filename, folder, text_repr)) {
    return -1;
  }
  if (NULL == (file = fopen(filename, "w+"))) {
    sp_log_err("request_logging", "Unable to open %s: %s", filename,
               strerror(errno));
    return -1;
  }

  fprintf(file, "RULE: sp.disable_function%s\n", text_repr);

  fprintf(file, "FILE: %s:%d\n", current_filename, current_line);
  for (size_t i = 0; i < (sizeof(zones) / sizeof(zones[0])) - 1; i++) {
    zval* variable_value;
    zend_string* variable_key;

    if (Z_TYPE(PG(http_globals)[zones[i].key]) == IS_UNDEF) {
      continue;
    }

    const HashTable* const ht = Z_ARRVAL(PG(http_globals)[zones[i].key]);
    fprintf(file, "%s:", zones[i].str);
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, variable_key, variable_value) {
      smart_str a = {0};

      php_var_export_ex(variable_value, 1, &a);
      ZSTR_VAL(a.s)[ZSTR_LEN(a.s)] = '\0';
      fprintf(file, "%s=%s ", ZSTR_VAL(variable_key), ZSTR_VAL(a.s));
      zend_string_release(a.s);
    }
    ZEND_HASH_FOREACH_END();
    fputs("\n", file);
  }
  fclose(file);

#undef CAT_AND_DEC
  return 0;
}

static char* zv_str_to_char(zval* zv) {
  zval copy;

  ZVAL_ZVAL(&copy, zv, 1, 0);
  for (size_t i = 0; i < Z_STRLEN(copy); i++) {
    if (Z_STRVAL(copy)[i] == '\0') {
      Z_STRVAL(copy)[i] = '0';
    }
  }
  return estrdup(Z_STRVAL(copy));
}

char* sp_convert_to_string(zval* zv) {
  switch (Z_TYPE_P(zv)) {
    case IS_FALSE:
      return estrdup("FALSE");
    case IS_TRUE:
      return estrdup("TRUE");
    case IS_NULL:
      return estrdup("NULL");
    case IS_LONG: {
      char* msg;
      spprintf(&msg, 0, ZEND_LONG_FMT, Z_LVAL_P(zv));
      return msg;
    }
    case IS_DOUBLE: {
      char* msg;
      spprintf(&msg, 0, "%f", Z_DVAL_P(zv));
      return msg;
    }
    case IS_STRING: {
      return zv_str_to_char(zv);
    }
    case IS_OBJECT:
      return estrdup("OBJECT");
    case IS_ARRAY:
      return estrdup("ARRAY");
    case IS_RESOURCE:
      return estrdup("RESOURCE");
  }
  return estrdup("");
}

bool sp_match_value(const char* value, const char* to_match, const pcre* rx) {
  if (to_match) {
    if (0 == strcmp(to_match, value)) {
      return true;
    }
  } else if (rx) {
    return is_regexp_matching(rx, value);
  } else {
    return true;
  }
  return false;
}

void sp_log_disable(const char* restrict path, const char* restrict arg_name,
                    const char* restrict arg_value,
                    const sp_disabled_function* config_node) {
  const char* dump = config_node->dump;
  const char* alias = config_node->alias;
  const int sim = config_node->simulation;
  if (arg_name) {
    if (alias) {
      sp_log_msg(
          "disabled_function", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
          "The call to the function '%s' in %s:%d has been disabled, "
          "because its argument '%s' content (%s) matched the rule '%s'.",
          path, zend_get_executed_filename(TSRMLS_C),
          zend_get_executed_lineno(TSRMLS_C), arg_name,
          arg_value ? arg_value : "?", alias);
    } else {
      sp_log_msg("disabled_function", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
                 "The call to the function '%s' in %s:%d has been disabled, "
                 "because its argument '%s' content (%s) matched a rule.",
                 path, zend_get_executed_filename(TSRMLS_C),
                 zend_get_executed_lineno(TSRMLS_C), arg_name,
                 arg_value ? arg_value : "?");
    }
  } else {
    if (alias) {
      sp_log_msg("disabled_function", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
                 "The call to the function '%s' in %s:%d has been disabled, "
                 "because of the the rule '%s'.",
                 path, zend_get_executed_filename(TSRMLS_C),
                 zend_get_executed_lineno(TSRMLS_C), alias);
    } else {
      sp_log_msg("disabled_function", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
                 "The call to the function '%s' in %s:%d has been disabled.",
                 path, zend_get_executed_filename(TSRMLS_C),
                 zend_get_executed_lineno(TSRMLS_C));
    }
  }
  if (dump) {
    sp_log_request(config_node->dump, config_node->textual_representation);
  }
}

void sp_log_disable_ret(const char* restrict path,
                        const char* restrict ret_value,
                        const sp_disabled_function* config_node) {
  const char* dump = config_node->dump;
  const char* alias = config_node->alias;
  const int sim = config_node->simulation;
  if (alias) {
    sp_log_msg(
        "disabled_function", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
        "The execution has been aborted in %s:%d, "
        "because the function '%s' returned '%s', which matched the rule '%s'.",
        zend_get_executed_filename(TSRMLS_C),
        zend_get_executed_lineno(TSRMLS_C), path, ret_value ? ret_value : "?",
        alias);
  } else {
    sp_log_msg(
        "disabled_function", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
        "The execution has been aborted in %s:%d, "
        "because the return value (%s) of the function '%s' matched a rule.",
        zend_get_executed_filename(TSRMLS_C),
        zend_get_executed_lineno(TSRMLS_C), ret_value ? ret_value : "?", path);
  }
  if (dump) {
    sp_log_request(dump, config_node->textual_representation);
  }
}

bool sp_match_array_key(const zval* zv, const char* to_match, const pcre* rx) {
  zend_string* key;
  zend_ulong idx;

  ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(zv), idx, key) {
    if (key) {
      if (sp_match_value(ZSTR_VAL(key), to_match, rx)) {
        return true;
      }
    } else {
      char* idx_str = NULL;
      spprintf(&idx_str, 0, "%lu", idx);
      if (sp_match_value(idx_str, to_match, rx)) {
        efree(idx_str);
        return true;
      }
      efree(idx_str);
    }
  }
  ZEND_HASH_FOREACH_END();
  return false;
}

bool sp_match_array_value(const zval* arr, const char* to_match,
                          const pcre* rx) {
  zval* value;

  ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), value) {
    if (Z_TYPE_P(value) != IS_ARRAY) {
      char* value_str = sp_convert_to_string(value);
      if (sp_match_value(value_str, to_match, rx)) {
        efree(value_str);
        return true;
      } else {
        efree(value_str);
      }
    } else if (sp_match_array_value(value, to_match, rx)) {
      return true;
    }
  }
  ZEND_HASH_FOREACH_END();
  return false;
}

int hook_function(const char* original_name, HashTable* hook_table,
                  void (*new_function)(INTERNAL_FUNCTION_PARAMETERS),
                  bool hook_execution_table) {
  zend_internal_function* func;
  HashTable* ht =
      hook_execution_table == true ? EG(function_table) : CG(function_table);

  /* The `mb` module likes to hook functions, like strlen->mb_strlen,
   * so we have to hook both of them. */

  if ((func = zend_hash_str_find_ptr(ht, VAR_AND_LEN(original_name)))) {
    if (func->handler == new_function) {
      return SUCCESS;
    }
  }

  if ((func = zend_hash_str_find_ptr(CG(function_table),
                                     VAR_AND_LEN(original_name)))) {
    if (func->handler == new_function) {
      /* Success !*/
    } else if (zend_hash_str_add_new_ptr((hook_table),
                                         VAR_AND_LEN(original_name),
                                         func->handler) == NULL) {
      sp_log_err("function_pointer_saving",
                 "Could not save function pointer for %s", original_name);
      return FAILURE;
    } else {
      func->handler = new_function;
    }
  }

  if (0 == strncmp(original_name, "mb_", 3)) {
    CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
    if (zend_hash_str_find(ht, VAR_AND_LEN(original_name + 3))) {
      hook_function(original_name + 3, hook_table, new_function,
                    hook_execution_table);
    }
  } else {  // TODO this can be moved somewhere else to gain some marginal perfs
    CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
    char* mb_name = ecalloc(strlen(original_name) + 3 + 1, 1);
    memcpy(mb_name, "mb_", 3);
    memcpy(mb_name + 3, VAR_AND_LEN(original_name));
    if (zend_hash_str_find(CG(function_table), VAR_AND_LEN(mb_name))) {
      hook_function(mb_name, hook_table, new_function, hook_execution_table);
    }
  }

  return SUCCESS;
}

int hook_regexp(const pcre* regexp, HashTable* hook_table,
                void (*new_function)(INTERNAL_FUNCTION_PARAMETERS),
                bool hook_execution_table) {
  zend_string* key;
  HashTable* ht =
      hook_execution_table == true ? EG(function_table) : CG(function_table);

  ZEND_HASH_FOREACH_STR_KEY(ht, key) {
    if (key) {
      int vec[30];
      int ret = sp_pcre_exec(regexp, NULL, key->val, key->len, 0, 0, vec,
                             sizeof(vec) / sizeof(int));
      if (ret < 0) { /* Error or no match*/
        if (PCRE_ERROR_NOMATCH != ret) {
          sp_log_err("pcre", "Runtime error with pcre, error code: %d", ret);
          return FAILURE;
        }
        continue;
      }
      hook_function(key->val, hook_table, new_function, hook_execution_table);
    }
  }
  ZEND_HASH_FOREACH_END();
  return SUCCESS;
}

bool check_is_in_eval_whitelist(const char* const function_name) {
  const sp_list_node* it = SNUFFLEUPAGUS_G(config).config_eval->whitelist;

  if (!it) {
    return false;
  }

  /* yes, we could use a HashTable instead, but since the list is pretty
   * small, it doesn't maka a difference in practise. */
  while (it && it->data) {
    if (0 == strcmp(function_name, (char*)(it->data))) {
      /* We've got a match, the function is whiteslited. */
      return true;
    }
    it = it->next;
  }
  return false;
}
