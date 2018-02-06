#include "php_snuffleupagus.h"
#include "rfc1867.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

#define EFREE_3(env)               \
  for (size_t i = 0; i < 4; i++) { \
    efree(env[i]);                 \
  }

void hook_upload() {
  sp_rfc1867_orig_callback = php_rfc1867_callback;
  php_rfc1867_callback = sp_rfc1867_callback;
}

int sp_rfc1867_callback(unsigned int event, void *event_data, void **extra) {
  int retval = SUCCESS;

  if (sp_rfc1867_orig_callback) {
    retval = sp_rfc1867_orig_callback(event, event_data, extra);
  }

  if (event == MULTIPART_EVENT_END) {
    zend_string *file_key __attribute__((unused)) = NULL;
    zval *file;
    pid_t pid;

    sp_log_debug(
        "Got %d files",
        zend_hash_num_elements(Z_ARRVAL(PG(http_globals)[TRACK_VARS_FILES])));

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(PG(http_globals)[TRACK_VARS_FILES]),
                                  file_key, file) {  // for each uploaded file

      char *filename =
          Z_STRVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), "name", 4));
      char *tmp_name =
          Z_STRVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), "tmp_name", 8));
      size_t filesize =
          Z_LVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), "size", 4));
      char *cmd[3] = {0};
      char *env[5] = {0};

      sp_log_debug("Filename: %s\nTmpname: %s\nSize: %d\nError: %d\nScript: %s",
                   filename, tmp_name, filesize,
                   Z_LVAL_P(zend_hash_str_find(Z_ARRVAL_P(file), "error", 5)),
                   SNUFFLEUPAGUS_G(config).config_upload_validation->script);

      cmd[0] = SNUFFLEUPAGUS_G(config).config_upload_validation->script;
      cmd[1] = tmp_name;
      cmd[2] = NULL;

      spprintf(&env[0], 0, "SP_FILENAME=%s", filename);
      spprintf(&env[1], 0, "SP_REMOTE_ADDR=%s", getenv("REMOTE_ADDR"));
      spprintf(&env[2], 0, "SP_CURRENT_FILE=%s",
               zend_get_executed_filename(TSRMLS_C));
      spprintf(&env[3], 0, "SP_FILESIZE=%zu", filesize);
      env[4] = NULL;

      if ((pid = fork()) == 0) {
        if (execve(SNUFFLEUPAGUS_G(config).config_upload_validation->script,
                   cmd, env) == -1) {
          sp_log_err("upload_validation", "Could not call '%s' : %s",
                     SNUFFLEUPAGUS_G(config).config_upload_validation->script,
                     strerror(errno));
          EFREE_3(env);
          exit(1);
        }
      } else if (pid == -1) {
        // LCOV_EXCL_START
        sp_log_err("upload_validation", "Could not fork process : %s\n",
                   strerror(errno));
        EFREE_3(env);
        continue;
        // LCOV_EXCL_STOP
      }

      EFREE_3(env);
      int waitstatus;
      wait(&waitstatus);
      if (WEXITSTATUS(waitstatus) != 0) {  // Nope
        char *uri = getenv("REQUEST_URI");
        int sim = SNUFFLEUPAGUS_G(config).config_upload_validation->simulation;
        sp_log_msg("upload_validation", sim ? SP_LOG_SIMULATION : SP_LOG_DROP,
                   "The upload of %s on %s was rejected.", filename,
                   uri ? uri : "?");
        if (!SNUFFLEUPAGUS_G(config).config_upload_validation->simulation) {
          zend_bailout();
        }
      }
    }
    ZEND_HASH_FOREACH_END();
  }
  return retval;
}
