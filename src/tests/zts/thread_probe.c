#include "php.h"
#include "main/php_main.h"
#include "zend_exceptions.h"
#include <pthread.h>
#include <stdbool.h>

ZEND_TSRMLS_CACHE_DEFINE()

typedef struct {
  pthread_mutex_t lock;
  pthread_barrier_t barrier;
  bool abort;
} sp_test_gate;

typedef struct {
  const char *before;
  const char *after;
  sp_test_gate *gate;
  bool first;
  bool success;
} sp_test_worker;

static bool sp_test_eval(const char *script) {
  bool success = false;
  char *expression;
  spprintf(&expression, 0, "(static function () { %s })()", script);
  zval result;
  ZVAL_UNDEF(&result);
  zend_try {
    success = zend_eval_string(expression, &result, "ZTS regression test") == SUCCESS &&
              !EG(exception) && Z_TYPE(result) == IS_TRUE;
  } zend_catch {
    success = false;
  } zend_end_try();
  efree(expression);
  zval_ptr_dtor(&result);
  if (EG(exception)) {
    zend_clear_exception();
  }
  return success;
}

static void *sp_test_thread(void *arg) {
  sp_test_worker *worker = arg;
  pthread_mutex_lock(&worker->gate->lock);
  bool abort = worker->gate->abort;
  pthread_mutex_unlock(&worker->gate->lock);
  if (abort) {
    return NULL;
  }
  ts_resource(0);
  ZEND_TSRMLS_CACHE_UPDATE();
  bool started = false;
  zend_try {
    started = php_request_startup() == SUCCESS;
  } zend_catch {
    started = false;
  } zend_end_try();
  worker->success = started;
  if (started && worker->first) {
    worker->success = sp_test_eval(worker->before);
  }
  pthread_barrier_wait(&worker->gate->barrier);
  if (started && !worker->first) {
    worker->success = sp_test_eval(worker->after);
  }
  pthread_barrier_wait(&worker->gate->barrier);
  if (started && worker->first && worker->success) {
    worker->success = sp_test_eval(worker->after);
  }
  if (started) {
    php_request_shutdown(NULL);
  }
  ts_free_thread();
  return NULL;
}

PHP_FUNCTION(sp_zts_reload) {
  char *path;
  size_t length;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(path, length)
  ZEND_PARSE_PARAMETERS_END();
  zend_string *name = zend_string_init("sp.configuration_file", sizeof("sp.configuration_file") - 1, 0);
  int status = zend_alter_ini_entry_chars(name, path, length, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_string_release(name);
  if (status == FAILURE) {
    zend_throw_error(NULL, "Cannot load threaded test configuration");
    RETURN_FALSE;
  }
  RETURN_TRUE;
}

PHP_FUNCTION(sp_zts_run) {
  char *before, *after, *other;
  size_t before_len, after_len, other_len;
  ZEND_PARSE_PARAMETERS_START(3, 3)
    Z_PARAM_STRING(before, before_len)
    Z_PARAM_STRING(after, after_len)
    Z_PARAM_STRING(other, other_len)
  ZEND_PARSE_PARAMETERS_END();
  sp_test_gate gate = {.lock = PTHREAD_MUTEX_INITIALIZER, .abort = false};
  if (pthread_barrier_init(&gate.barrier, NULL, 2) != 0) {
    pthread_mutex_destroy(&gate.lock);
    zend_throw_error(NULL, "Cannot initialize threaded test barrier");
    RETURN_FALSE;
  }
  sp_test_worker workers[] = {
      {before, after, &gate, true, false},
      {NULL, other, &gate, false, false}};
  pthread_t threads[2];
  size_t created = 0;
  pthread_mutex_lock(&gate.lock);
  for (; created < 2; created++) {
    if (pthread_create(&threads[created], NULL, sp_test_thread, &workers[created]) != 0) {
      gate.abort = true;
      break;
    }
  }
  pthread_mutex_unlock(&gate.lock);
  for (size_t i = 0; i < created; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_barrier_destroy(&gate.barrier);
  pthread_mutex_destroy(&gate.lock);
  if (created != 2) {
    zend_throw_error(NULL, "Cannot start threaded test worker");
    RETURN_FALSE;
  }
  if (!workers[0].success || !workers[1].success) {
    zend_throw_error(NULL, "Threaded regression test failed (protected=%d, other=%d)",
                     workers[0].success, workers[1].success);
    RETURN_FALSE;
  }
  RETURN_TRUE;
}

#if PHP_VERSION_ID < 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sp_zts_reload, 0, 1, _IS_BOOL, NULL, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sp_zts_reload, 0, 1, _IS_BOOL, 0)
#endif
  ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sp_zts_run, 0, 3, _IS_BOOL, NULL, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sp_zts_run, 0, 3, _IS_BOOL, 0)
#endif
  ZEND_ARG_TYPE_INFO(0, before, IS_STRING, 0)
  ZEND_ARG_TYPE_INFO(0, after, IS_STRING, 0)
  ZEND_ARG_TYPE_INFO(0, other, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry sp_zts_test_functions[] = {
    PHP_FE(sp_zts_reload, arginfo_sp_zts_reload)
    PHP_FE(sp_zts_run, arginfo_sp_zts_run)
    PHP_FE_END};

static PHP_MINIT_FUNCTION(sp_zts_test) {
  ZEND_TSRMLS_CACHE_UPDATE();
  return SUCCESS;
}

zend_module_entry sp_zts_test_module_entry = {
    STANDARD_MODULE_HEADER, "sp_zts_test", sp_zts_test_functions,
    PHP_MINIT(sp_zts_test), NULL, NULL, NULL, NULL, "1", STANDARD_MODULE_PROPERTIES};

ZEND_GET_MODULE(sp_zts_test)
