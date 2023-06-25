#include "php_snuffleupagus.h"
#include "ext/standard/php_string.h"

static void (*orig_execute_ex)(zend_execute_data *execute_data) = NULL;
static void (*orig_zend_execute_internal)(zend_execute_data *execute_data,
                                          zval *return_value) = NULL;
#if PHP_VERSION_ID < 80100
static int (*orig_zend_stream_open)(const char *filename, zend_file_handle *handle) = NULL;
#else
static zend_result (*orig_zend_stream_open)(zend_file_handle *handle) = NULL;
#endif

// FIXME handle symlink
ZEND_COLD static inline void terminate_if_writable(char const* const filename) {
  sp_config_readonly_exec const* const config_ro_exec = &(SPCFG(readonly_exec));
  char const *errmsg = "unknown access problem";

  if (0 == access(filename, W_OK)) {
    errmsg = "Attempted execution of a writable file";
    goto violation;
  }
  if (errno != EACCES && errno != EROFS && errno != EPERM) {
    goto err;
  }

  // other checks are 'extended checks' that can be enabled/disabled via config
  if (!config_ro_exec->extended_checks) {
    return;
  }

  struct stat buf;
  if (0 != stat(filename, &buf)) {
    goto err;
  }
  if (buf.st_uid == geteuid()) {
    errmsg = "Attempted execution of a file owned by the PHP process";
    goto violation;
  }

  char *const dirname = estrndup(filename, strlen(filename));
  php_dirname(dirname, strlen(dirname));
  if (0 == access(dirname, W_OK)) {
    errmsg = "Attempted execution of a file in a writable directory";

    efree(dirname);
    goto violation;
  }
  if (errno != EACCES && errno != EROFS && errno != EPERM) {
    efree(dirname);
    goto err;
  }

  if (0 != stat(dirname, &buf)) {
    efree(dirname);
    goto err;
  }
  efree(dirname);
  if (buf.st_uid == geteuid()) {
    errmsg = "Attempted execution of a file in directory owned by the PHP process";
    goto violation;
  }

  return;

violation:
  if (config_ro_exec->dump) {
    sp_log_request(config_ro_exec->dump, config_ro_exec->textual_representation);
  }
  if (config_ro_exec->simulation) {
    sp_log_simulation("readonly_exec", "%s (%s)", errmsg, filename);
  } else {
    sp_log_drop("readonly_exec", "%s (%s)", errmsg, filename);
  }
  return;

err:
  sp_log_err("readonly_exec", "Error while accessing %s: %s", filename, strerror(errno));
}

inline static void is_builtin_matching(
    const zend_string *restrict const param_value,
    const char *restrict const function_name,
    const char *restrict const param_name, const sp_list_node *config,
    const HashTable *ht) {
  if (!config || !config->data) {
    return;
  }

  should_disable_ht(EG(current_execute_data), function_name, param_value, param_name, SPCFG(disabled_functions_reg).disabled_functions, ht);
}

static void is_in_eval_and_whitelisted(zend_execute_data const* const execute_data) {
  sp_config_eval const* const config_eval = &(SPCFG(eval));

  if (EXPECTED(0 == SPG(in_eval))) {
    return;
  }

  if (EXPECTED(NULL == config_eval->whitelist)) {
    return;
  }

  if (zend_is_executing() && !EX(func)) {
    return;  // LCOV_EXCL_LINE
  }

  char *function_name = get_complete_function_path(execute_data);
  if (!function_name) {
    return;
  }

  if (UNEXPECTED(false == check_is_in_eval_whitelist(function_name))) {
    if (config_eval->dump) {
      sp_log_request(config_eval->dump, config_eval->textual_representation);
    }
    if (config_eval->simulation) {
      sp_log_simulation("Eval_whitelist", "The function '%s' isn't in the eval whitelist, logging its call.", function_name);
      goto out;
    } else {
      sp_log_drop("Eval_whitelist", "The function '%s' isn't in the eval whitelist, dropping its call.", function_name);
    }
  }

out:
  efree(function_name);
}

/* This function gets the filename in which `eval()` is called from,
 * since it looks like "foo.php(1) : eval()'d code", so we're starting
 * from the end of the string until the second closing parenthesis. */
zend_string *get_eval_filename(const char *const filename) {
  int count = 0;
  zend_string *clean_filename = zend_string_init(filename, strlen(filename), 0);

  for (int i = ZSTR_LEN(clean_filename); i >= 0; i--) {
    if (ZSTR_VAL(clean_filename)[i] == '(') {
      if (count == 1) {
        ZSTR_VAL(clean_filename)[i] = '\0';
        clean_filename = zend_string_truncate(clean_filename, i, 0);
        break;
      }
      count++;
    }
  }
  return clean_filename;
}

static inline void sp_orig_execute(zend_execute_data *execute_data) {
  SPG(execution_depth)++;
  if (SPCFG(max_execution_depth) > 0 && SPG(execution_depth) > SPCFG(max_execution_depth)) {
    sp_log_drop("execute", "Maximum recursion limit reached. Script terminated.");
  }
  orig_execute_ex(execute_data);
  SPG(execution_depth)--;
}

static inline void sp_check_writable(zend_execute_data *execute_data) {
  if (execute_data && EX(func) && EX(func)->op_array.filename && SPCFG(readonly_exec).enable) {
    terminate_if_writable(ZSTR_VAL(EX(func)->op_array.filename));
  }
}

static inline void sp_call_orig_execute(INTERNAL_FUNCTION_PARAMETERS, bool internal) {
  if (internal) {
    if (UNEXPECTED(NULL != orig_zend_execute_internal)) {
      orig_zend_execute_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    } else {
      EX(func)->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
  } else {
    sp_orig_execute(execute_data);
  }
}

static inline void sp_execute_handler(INTERNAL_FUNCTION_PARAMETERS, bool internal) {
  if (!execute_data) {
    return;  // LCOV_EXCL_LINE
  }

  is_in_eval_and_whitelisted(execute_data);

  if (!internal) {
    if (UNEXPECTED(EX(func)->op_array.type == ZEND_EVAL_CODE)) {
      sp_list_node const* const config = zend_hash_str_find_ptr(SPCFG(disabled_functions), ZEND_STRL("eval"));

#if PHP_VERSION_ID >= 80000
      is_builtin_matching(SPG(eval_source_string), "eval", "code", config, SPCFG(disabled_functions));
#else
      is_builtin_matching(Z_STR_P(SPG(eval_source_string)), "eval", "code", config, SPCFG(disabled_functions));
#endif

      SPG(in_eval)++;
      sp_orig_execute(execute_data);
      SPG(in_eval)--;
      return;
    }

    sp_check_writable(execute_data);
  }

  if (!SPG(hook_execute)) {
    sp_call_orig_execute(INTERNAL_FUNCTION_PARAM_PASSTHRU, internal);
    return;
  }

  char *function_name = get_complete_function_path(execute_data);

  if (!function_name) {
    sp_call_orig_execute(INTERNAL_FUNCTION_PARAM_PASSTHRU, internal);
    return;
  }

  bool is_hooked = (zend_hash_str_find(SPG(disabled_functions_hook), VAR_AND_LEN(function_name)) || zend_hash_str_find(SPG(disabled_functions_hook), VAR_AND_LEN(function_name)));
  if (is_hooked) {
    sp_call_orig_execute(INTERNAL_FUNCTION_PARAM_PASSTHRU, internal);
    return;
  }

  // If we're at an internal function
  if (!execute_data->prev_execute_data ||
      !execute_data->prev_execute_data->func ||
      !ZEND_USER_CODE(execute_data->prev_execute_data->func->type) ||
      !execute_data->prev_execute_data->opline) {
    should_disable_ht(execute_data, function_name, NULL, NULL, SPCFG(disabled_functions_reg).disabled_functions, SPCFG(disabled_functions));
  } else {  // If we're at a userland function call
    switch (execute_data->prev_execute_data->opline->opcode) {
      case ZEND_DO_FCALL:
      case ZEND_DO_FCALL_BY_NAME:
      case ZEND_DO_ICALL:
      case ZEND_DO_UCALL:
      case ZEND_TICKS:
        should_disable_ht(execute_data, function_name, NULL, NULL, SPCFG(disabled_functions_reg).disabled_functions, SPCFG(disabled_functions));
      default:
        break;
    }
  }

  // When a function's return value isn't used, php doesn't store it in the
  // execute_data, so we need to use a local variable to be able to match on
  // it later.
  zval ret_val;
  if (EX(return_value) == NULL && return_value == NULL) {
    memset(&ret_val, 0, sizeof(ret_val));
    return_value = EX(return_value) = &ret_val;
  }

  sp_call_orig_execute(INTERNAL_FUNCTION_PARAM_PASSTHRU, internal);

  should_drop_on_ret_ht(return_value, function_name, SPCFG(disabled_functions_reg_ret).disabled_functions, SPCFG(disabled_functions_ret), execute_data);

  efree(function_name);

  if (EX(return_value) == &ret_val) {
    return_value = EX(return_value) = NULL;
  }
}

static void sp_execute_ex(zend_execute_data *execute_data) {
  sp_execute_handler(execute_data, execute_data ? EX(return_value) : NULL, false);
}

static void sp_zend_execute_internal(INTERNAL_FUNCTION_PARAMETERS) {
  sp_execute_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static inline void sp_stream_open_checks(zend_string *zend_filename, zend_file_handle *handle) {
  zend_execute_data const *const data = EG(current_execute_data);

  if ((NULL == data) || (NULL == data->opline) ||
      (data->func->type != ZEND_USER_FUNCTION)) {
    return;
  }

  HashTable const* const disabled_functions_hooked = SPCFG(disabled_functions_hooked);

  switch (data->opline->opcode) {
    case ZEND_INCLUDE_OR_EVAL:
      if (SPCFG(readonly_exec).enable) {
        char *fn = ZSTR_VAL(zend_filename);
        if (ZSTR_LEN(zend_filename) >= strlen("file://") && memcmp(fn, "file://", strlen("file://")) == 0) {
          fn += strlen("file://");
        } else if (!php_memnstr(ZSTR_VAL(zend_filename), "://", strlen("://"), ZSTR_VAL(zend_filename) + ZSTR_LEN(zend_filename))) {
          // ignore stream wrappers other than file:// for now
          terminate_if_writable(fn);
        }
      }
      switch (data->opline->extended_value) {
        case ZEND_INCLUDE:
          is_builtin_matching(
              zend_filename, "include", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, ZEND_STRL("include")),
              disabled_functions_hooked);
          break;
        case ZEND_REQUIRE:
          is_builtin_matching(
              zend_filename, "require", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, ZEND_STRL("require")),
              disabled_functions_hooked);
          break;
        case ZEND_REQUIRE_ONCE:
          is_builtin_matching(
              zend_filename, "require_once", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, ZEND_STRL("require_once")),
              disabled_functions_hooked);
          break;
        case ZEND_INCLUDE_ONCE:
          is_builtin_matching(
              zend_filename, "include_once", "inclusion path",
              zend_hash_str_find_ptr(disabled_functions_hooked, ZEND_STRL("include_once")),
              disabled_functions_hooked);
          break;
          EMPTY_SWITCH_DEFAULT_CASE();  // LCOV_EXCL_LINE
      }
  }
}

#if PHP_VERSION_ID < 80100

static int sp_stream_open(const char *filename, zend_file_handle *handle) {
  zend_string *zend_filename = zend_string_init(filename, strlen(filename), 0);

  sp_stream_open_checks(zend_filename, handle);

  zend_string_release_ex(zend_filename, 0);
  return orig_zend_stream_open(filename, handle);
}

#else // PHP >= 8.1

static zend_result sp_stream_open(zend_file_handle *handle) {
  sp_stream_open_checks(handle->filename, handle);
  return orig_zend_stream_open(handle);
}

#endif

ZEND_API zend_op_array* (*orig_zend_compile_file)(zend_file_handle* file_handle,
                                                  int type) = NULL;
#if PHP_VERSION_ID >= 80200
ZEND_API zend_op_array* (*orig_zend_compile_string)(
    zend_string* source_string, const char* filename,
    enum _zend_compile_position position) = NULL;
#elif PHP_VERSION_ID >= 80000
ZEND_API zend_op_array* (*orig_zend_compile_string)(
    zend_string* source_string, const char* filename) = NULL;
#else
ZEND_API zend_op_array* (*orig_zend_compile_string)(zval* source_string,
                                                    char* filename) = NULL;
#endif

#if PHP_VERSION_ID >= 80200
ZEND_API zend_op_array* sp_compile_string(zend_string* source_string,
                                          const char* filename,
					  enum _zend_compile_position position) {
#elif PHP_VERSION_ID >= 80000
ZEND_API zend_op_array* sp_compile_string(zend_string* source_string,
                                          const char* filename) {
#else
ZEND_API zend_op_array* sp_compile_string(zval* source_string, char* filename) {
#endif
  // TODO(jvoisin) handle recursive calls to `eval`
  SPG(eval_source_string) = source_string;
  zend_op_array* opline = orig_zend_compile_string(source_string, filename
#if PHP_VERSION_ID >= 80200
    , position
#endif
    );
  if (SPCFG(sloppy).enable) {
    sp_sloppy_modify_opcode(opline);
  }
  return opline;
}

ZEND_API zend_op_array* sp_compile_file(zend_file_handle* file_handle,
                                        int type) {
  zend_op_array* opline = orig_zend_compile_file(file_handle, type);
  if (SPCFG(sloppy).enable) {
    sp_sloppy_modify_opcode(opline);
  }
  return opline;
}

int hook_execute(void) {
  TSRMLS_FETCH();

  if (NULL == orig_execute_ex && NULL == orig_zend_stream_open) {
    if (zend_execute_ex != sp_execute_ex) {
      /* zend_execute_ex is used for "user" function calls */
      orig_execute_ex = zend_execute_ex;
      zend_execute_ex = sp_execute_ex;
    }

    if (zend_execute_internal != sp_zend_execute_internal) {
      /* zend_execute_internal is used for "builtin" functions calls */
      orig_zend_execute_internal = zend_execute_internal;
      zend_execute_internal = sp_zend_execute_internal;
    }

    if (zend_stream_open_function != sp_stream_open) {
      /* zend_stream_open_function is used for include-related stuff */
      orig_zend_stream_open = zend_stream_open_function;
      zend_stream_open_function = sp_stream_open;
    }
  }

  if (NULL == orig_zend_compile_file && zend_compile_file != sp_compile_file) {
    orig_zend_compile_file = zend_compile_file;
    zend_compile_file = sp_compile_file;
  }

  if (NULL == orig_zend_compile_string &&
      zend_compile_string != sp_compile_string) {
    orig_zend_compile_string = zend_compile_string;
    zend_compile_string = sp_compile_string;
  }

  return SUCCESS;
}
