#include "php_snuffleupagus.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static inline void _sp_log_err(const char* fmt, ...) {
  char* msg;
  va_list args;

  va_start(args, fmt);
  vspprintf(&msg, 0, fmt, args);
  va_end(args);
  php_log_err(msg);
}

void sp_log_msg(char const *feature, char const *level, const char* fmt, ...) {
  char* msg;
  va_list args;

  va_start(args, fmt);
  vspprintf(&msg, 0, fmt, args);
  va_end(args);

  char const * const client_ip = sp_getenv("REMOTE_ADDR");
  _sp_log_err("[snuffleupagus][%s][%s][%s] %s", client_ip?client_ip:"0.0.0.0",
    feature, level, msg);
}


zend_always_inline char* sp_getenv(char* var) {
  if (sapi_module.getenv) {
    return sapi_module.getenv(ZEND_STRL(var));
  } else {
    return getenv(var);
  }
}

zend_always_inline int is_regexp_matching(const pcre* regexp, const char* str) {
  int vec[30];
  int ret = 0;

  assert(NULL != regexp);
  assert(NULL != str);

  ret = sp_pcre_exec(regexp, NULL, str, strlen(str), 0, 0, vec,
   sizeof(vec)/sizeof(int));

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
    sp_log_err("hash_computation", "Can not open the file %s to compute its hash.\n", filename);
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

static int construct_filename(char* filename, const char* folder) {
  time_t t = time(NULL);
  struct tm* tm = localtime(&t);  // FIXME use `localtime_r` instead
  struct timeval tval;

  if (0 > mkdir(folder, 0700) && errno != EEXIST) {
    sp_log_err("request_logging", "Unable to create the folder '%s'.",
      folder);
  }

  memcpy(filename, folder, strlen(folder));
  strcat(filename, "sp_dump_");
  strftime(filename + strlen(filename), 27, "%F_%T:", tm);
  gettimeofday(&tval, NULL);
  sprintf(filename + strlen(filename), "%04ld", tval.tv_usec);
  strcat(filename, "_");

  char* remote_addr = getenv("REMOTE_ADDR");
  if (remote_addr) { // ipv6: 8*4 bytes + 7 colons = 39 chars max
    strncat(filename, remote_addr, 40);
  } else {
    strcat(filename, "0.0.0.0");
  }
  strcat(filename, ".dump");

  return 0;
}

int sp_log_request(const char* folder) {
  FILE* file;
  const char* current_filename = zend_get_executed_filename(TSRMLS_C);
  const int current_line = zend_get_executed_lineno(TSRMLS_C);
  char filename[MAX_FOLDER_LEN] = {0};
  const struct {
    const char* str;
    const int key;
  } zones[] = {{"GET", TRACK_VARS_GET},       {"POST", TRACK_VARS_POST},
               {"COOKIE", TRACK_VARS_COOKIE}, {"SERVER", TRACK_VARS_SERVER},
               {"ENV", TRACK_VARS_ENV},       {NULL, 0}};

  if (0 != construct_filename(filename, folder)) {
    return -1;
  }
  if (NULL == (file = fopen(filename, "a"))) {
    sp_log_err("request_logging", "Unable to open %s", filename);
    return -1;
  }

  fprintf(file, "%s:%d\n", current_filename, current_line);
  for (size_t i = 0; i < (sizeof(zones) / sizeof(zones[0])) - 1; i++) {
    zval* variable_value;
    zend_string* variable_key;
    size_t params_len = strlen(zones[i].str) + 1;
    char* param;
    size_t size_max = 2048;

    if (Z_TYPE(PG(http_globals)[zones[i].key]) == IS_UNDEF) {
      continue;
    }

    const HashTable* ht = Z_ARRVAL(PG(http_globals)[zones[i].key]);

    // Compute the size of the allocation
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, variable_key, variable_value) {
      params_len += snprintf(NULL, 0, "%s=%s&", ZSTR_VAL(variable_key),
                             Z_STRVAL_P(variable_value));
    }
    ZEND_HASH_FOREACH_END();

    params_len = params_len>size_max?size_max:params_len;

#define NCAT_AND_DEC(a, b, c) strncat(a, b, c); c -= strlen(b);

    // Allocate and copy the data
    // FIXME Why are we even allocating?
    param = ecalloc(params_len, 1);
    NCAT_AND_DEC(param, zones[i].str, params_len);
    NCAT_AND_DEC(param, ":", params_len);
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, variable_key, variable_value) {
      NCAT_AND_DEC(param, ZSTR_VAL(variable_key), params_len);
      NCAT_AND_DEC(param, "=", params_len);
      NCAT_AND_DEC(param, Z_STRVAL_P(variable_value), params_len);
      NCAT_AND_DEC(param, "&", params_len);
    }
    ZEND_HASH_FOREACH_END();

    param[strlen(param) - 1] = '\0';

    fputs(param, file);
    fputs("\n", file);
    efree(param);
  }
  fclose(file);

#undef CAT_AND_DEC
  return 0;
}

static char *zv_str_to_char(zval *zv) {
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
        char *msg;
        spprintf(&msg, 0, ZEND_LONG_FMT, Z_LVAL_P(zv));
        return msg;
      }
    case IS_DOUBLE: {
        char *msg;
        spprintf(&msg, 0, "%f", Z_DVAL_P(zv));
        return msg;
      }
    case IS_STRING:{
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
      sp_log_msg("disabled_function", sim?SP_LOG_SIMULATION:SP_LOG_DROP,
          "The call to the function '%s' in %s:%d has been disabled, "
          "because its argument '%s' content (%s) matched the rule '%s'.",
          path, zend_get_executed_filename(TSRMLS_C),
          zend_get_executed_lineno(TSRMLS_C), arg_name, arg_value?arg_value:"?",
           alias);
    } else {
      sp_log_msg("disabled_function", sim?SP_LOG_SIMULATION:SP_LOG_DROP,
          "The call to the function '%s' in %s:%d has been disabled, "
          "because its argument '%s' content (%s) matched a rule.",
          path, zend_get_executed_filename(TSRMLS_C),
          zend_get_executed_lineno(TSRMLS_C), arg_name,
           arg_value?arg_value:"?");
    }
  } else {
    if (alias) {
      sp_log_msg("disabled_function", sim?SP_LOG_SIMULATION:SP_LOG_DROP,
          "The call to the function '%s' in %s:%d has been disabled, "
          "because of the the rule '%s'.",path,
          zend_get_executed_filename(TSRMLS_C),
          zend_get_executed_lineno(TSRMLS_C), alias);
    } else {
      sp_log_msg("disabled_function", sim?SP_LOG_SIMULATION:SP_LOG_DROP,
                 "The call to the function '%s' in %s:%d has been disabled.",
				 path, zend_get_executed_filename(TSRMLS_C),
                 zend_get_executed_lineno(TSRMLS_C));
    }
  }
  if (dump) {
    sp_log_request(config_node->dump);
  }
}

void sp_log_disable_ret(const char* restrict path,
                        const char* restrict ret_value,
                        const sp_disabled_function* config_node) {
  const char* dump = config_node->dump;
  const char* alias = config_node->alias;
  const int sim = config_node->simulation;
  if (alias) {
    sp_log_msg("disabled_function", sim?SP_LOG_SIMULATION:SP_LOG_DROP,
        "The execution has been aborted in %s:%d, "
        "because the function '%s' returned '%s', which matched the rule '%s'.",
        zend_get_executed_filename(TSRMLS_C),
        zend_get_executed_lineno(TSRMLS_C), path, ret_value?ret_value:"?", alias);
  } else {
    sp_log_msg("disabled_function", sim?SP_LOG_SIMULATION:SP_LOG_DROP,
        "The execution has been aborted in %s:%d, "
        "because the return value (%s) of the function '%s' matched a rule.",
        zend_get_executed_filename(TSRMLS_C),
        zend_get_executed_lineno(TSRMLS_C), ret_value?ret_value:"?", path);
  }
  if (dump) {
    sp_log_request(dump);
  }
}

int sp_match_array_key(const zval* zv, const char* to_match, const pcre* rx) {
  zend_string* key;
  zval* value;
  char* arg_value_str;

  ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zv), key, value) {
    if (Z_TYPE_P(value) == IS_ARRAY) {
      continue;
    }
    arg_value_str = sp_convert_to_string(value);
    if (!sp_match_value(arg_value_str, to_match, rx)) {
      efree(arg_value_str);
      continue;
    } else {
      efree(arg_value_str);
      return 1;
    }
  }
  ZEND_HASH_FOREACH_END();

  (void)key;  // silence a compiler warning

  return 0;
}

int sp_match_array_key_recurse(const zval* arr, sp_node_t* keys,
                               const char* to_match, const pcre* rx) {
  zend_string* key;
  zval* value;
  sp_node_t* current = keys;
  if (current == NULL) {
    return 0;
  }
  ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(arr), key, value) {
    if (Z_TYPE_P(value) == IS_ARRAY && !strcmp(ZSTR_VAL(key), current->data)) {
      return sp_match_array_key_recurse(value, current->next, to_match, rx);
    }
    if (!strcmp(ZSTR_VAL(key), current->data) && current->next == NULL) {
      if (!to_match && !rx) {
        return 1;
      }
      if (Z_TYPE_P(value) == IS_ARRAY) {
        return sp_match_array_key(value, to_match, rx);
      } else {
        char *value_str = sp_convert_to_string(value);
        if (sp_match_value(value_str, to_match, rx)) {
          efree(value_str);
          return 1;
        } else {
          efree (value_str);
          return 0;
        }
      }
    }
  }
  ZEND_HASH_FOREACH_END();
  return 0;
}


int hook_function(const char* original_name, HashTable* hook_table,
                  void (*new_function)(INTERNAL_FUNCTION_PARAMETERS),
                  bool hook_execution_table) {
  zend_internal_function* func;
  HashTable *ht = hook_execution_table==true?EG(function_table):CG(function_table);

  /* The `mb` module likes to hook functions, like strlen->mb_strlen,
   * so we have to hook both of them. */

  if ((func = zend_hash_str_find_ptr(ht,
                                     VAR_AND_LEN(original_name)))) {
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
    if (zend_hash_str_find(ht,
                           VAR_AND_LEN(original_name + 3))) {
      hook_function(original_name + 3, hook_table, new_function, hook_execution_table);
    }
  } else {  // TODO this can be moved somewhere else to gain some marginal perfs
    CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
    char* mb_name = pecalloc(strlen(original_name) + 3 + 1, 1, 0);
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
  HashTable *ht = hook_execution_table==true?EG(function_table):CG(function_table);

  ZEND_HASH_FOREACH_STR_KEY(ht, key) {
    if (key) {
      int vec[30];
      int ret = sp_pcre_exec(regexp, NULL, key->val, key->len, 0, 0, vec,
       sizeof(vec)/sizeof(int));
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
