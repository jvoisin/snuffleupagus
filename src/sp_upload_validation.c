#include "php_snuffleupagus.h"

int (*sp_rfc1867_orig_callback)(unsigned int event, void *event_data,
                                void **extra);
int sp_rfc1867_callback(unsigned int event, void *event_data, void **extra);

#define EFREE_3(env)               \
  for (size_t i = 0; i < 4; i++) { \
    efree(env[i]);                 \
  }

#ifdef PHP_WIN32

int sp_rfc1867_callback_win(unsigned int event, void *event_data,
                            void **extra) {
  sp_log_simulation(
      "upload_validation",
      "The upload validation doesn't work for now on Windows yet, "
      "see https://github.com/jvoisin/snuffleupagus/issues/248 for "
      "details.");
  return SUCCESS;
}

#else

int sp_rfc1867_callback(unsigned int event, void *event_data, void **extra) {
  int retval = SUCCESS;

  if (sp_rfc1867_orig_callback) {
    retval = sp_rfc1867_orig_callback(event, event_data, extra);
  }

  if (event == MULTIPART_EVENT_END) {
    zend_string *file_key __attribute__((unused)) = NULL;
    const sp_config_upload_validation *config_upload = &(SPCFG(upload_validation));
    zval *file;
    pid_t pid;

    sp_log_debug("Got %d files", zend_hash_num_elements(Z_ARRVAL(PG(http_globals)[TRACK_VARS_FILES])));

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(PG(http_globals)[TRACK_VARS_FILES]), file_key, file) {  // for each uploaded file

      char *filename = Z_STRVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), ZEND_STRL("name")));
      char *tmp_name = Z_STRVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), ZEND_STRL("tmp_name")));
      size_t filesize = Z_LVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), ZEND_STRL("size")));
      char *cmd[3] = {0};
      char *env[5] = {0};

      sp_log_debug("Filename: %s\nTmpname: %s\nSize: %zd\nError: %lld\nScript: %s",
                   filename, tmp_name, filesize,
                   Z_LVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), "error", 5)),
                   ZSTR_VAL(config_upload->script));

      cmd[0] = ZSTR_VAL(config_upload->script);
      cmd[1] = tmp_name;
      cmd[2] = NULL;

      spprintf(&env[0], 0, "SP_FILENAME=%s", filename);
      spprintf(&env[1], 0, "SP_REMOTE_ADDR=%s", getenv("REMOTE_ADDR"));
      spprintf(&env[2], 0, "SP_CURRENT_FILE=%s", zend_get_executed_filename(TSRMLS_C));
      spprintf(&env[3], 0, "SP_FILESIZE=%zu", filesize);
      env[4] = NULL;

      if ((pid = fork()) == 0) {
        if (execve(ZSTR_VAL(config_upload->script), cmd, env) == -1) {
          sp_log_warn("upload_validation", "Could not call '%s' : %s", ZSTR_VAL(config_upload->script), strerror(errno));
          EFREE_3(env);
          exit(1);
        }
      } else if (pid == -1) {
        // LCOV_EXCL_START
        sp_log_err("upload_validation", "Could not fork process : %s\n", strerror(errno));
        EFREE_3(env);
        continue;
        // LCOV_EXCL_STOP
      }

      EFREE_3(env);
      int waitstatus;
      wait(&waitstatus);
      if (WEXITSTATUS(waitstatus) != 0) {  // Nope
        char *uri = getenv("REQUEST_URI");
        int sim = config_upload->simulation;
        sp_log_auto("upload_validation", sim, "The upload of %s on %s was rejected.", filename, uri ? uri : "?");
      }
    }
    ZEND_HASH_FOREACH_END();
  }
  return retval;
}
#endif

void hook_upload() {
  if (php_rfc1867_callback == sp_rfc1867_callback) {
    return;
  }

  if (NULL == sp_rfc1867_orig_callback) {
    sp_rfc1867_orig_callback = php_rfc1867_callback;
    php_rfc1867_callback = sp_rfc1867_callback;
  }
}
