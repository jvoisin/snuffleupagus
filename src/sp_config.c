#include <errno.h>
#if defined(__FreeBSD__)
#define _WITH_GETLINE
#endif
#include <stdio.h>
#include <string.h>

#include "php_snuffleupagus.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

size_t sp_line_no;

sp_config_tokens const sp_func[] = {
    {.func = parse_unserialize, .token = SP_TOKEN_UNSERIALIZE_HMAC},
    {.func = parse_random, .token = SP_TOKEN_HARDEN_RANDOM},
    {.func = parse_disabled_functions, .token = SP_TOKEN_DISABLE_FUNC},
    {.func = parse_readonly_exec, .token = SP_TOKEN_READONLY_EXEC},
    {.func = parse_global_strict, .token = SP_TOKEN_GLOBAL_STRICT},
    {.func = parse_upload_validation, .token = SP_TOKEN_UPLOAD_VALIDATION},
    {.func = parse_cookie, .token = SP_TOKEN_COOKIE_ENCRYPTION},
    {.func = parse_global, .token = SP_TOKEN_GLOBAL},
    {.func = parse_auto_cookie_secure, .token = SP_TOKEN_AUTO_COOKIE_SECURE},
    {.func = parse_disable_xxe, .token = SP_TOKEN_DISABLE_XXE},
    {.func = parse_eval_blacklist, .token = SP_TOKEN_EVAL_BLACKLIST},
    {.func = parse_eval_whitelist, .token = SP_TOKEN_EVAL_WHITELIST},
    {.func = parse_session, .token = SP_TOKEN_SESSION_ENCRYPTION},
    {.func = parse_sloppy_comparison, .token = SP_TOKEN_SLOPPY_COMPARISON},
    {.func = parse_wrapper_whitelist, .token = SP_TOKEN_ALLOW_WRAPPERS},
    {NULL, NULL}};

/* Top level keyword parsing */

static int parse_line(char *line) {
  char *ptr = line;

  while (*ptr == ' ' || *ptr == '\t') {
    ++ptr;
  }

  if (!*ptr || *ptr == '#' || *ptr == ';') {
    return 0;
  }

  if (strncmp(ptr, SP_TOKEN_BASE, strlen(SP_TOKEN_BASE))) {
    sp_log_err("config", "Invalid configuration prefix for '%s' on line %zu",
               line, sp_line_no);
    return -1;
  }
  ptr += strlen(SP_TOKEN_BASE);

  for (size_t i = 0; sp_func[i].func; i++) {
    if (!strncmp(sp_func[i].token, ptr, strlen(sp_func[i].token))) {
      return sp_func[i].func(ptr + strlen(sp_func[i].token));
    }
  }
  sp_log_err("config", "Invalid configuration section '%s' on line %zu", line,
             sp_line_no);
  return -1;
}

/* keyword parsing */
int parse_empty(char *restrict line, char *restrict keyword, void *retval) {
  *(bool *)retval = true;
  return 0;
}

int parse_list(char *restrict line, char *restrict keyword, void *list_ptr) {
  zend_string *value = NULL;
  sp_list_node **list = list_ptr;
  char *token, *tmp;

  size_t consumed = 0;
  value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  if (!value) {
    return -1;
  }

  tmp = ZSTR_VAL(value);
  while ((token = strtok_r(tmp, ",", &tmp))) {
    *list = sp_list_insert(*list, zend_string_init(token, strlen(token), 1));
  }

  pefree(value, 1);
  return consumed;
}

int parse_php_type(char *restrict line, char *restrict keyword, void *retval) {
  size_t consumed = 0;
  zend_string *value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  if (value) {
    if (zend_string_equals_literal_ci(value, "undef")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_UNDEF;
    } else if (zend_string_equals_literal_ci(value, "null")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_NULL;
    } else if (zend_string_equals_literal_ci(value, "true")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_TRUE;
    } else if (zend_string_equals_literal_ci(value, "false")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_FALSE;
    } else if (zend_string_equals_literal_ci(value, "long")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_LONG;
    } else if (zend_string_equals_literal_ci(value, "double")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_DOUBLE;
    } else if (zend_string_equals_literal_ci(value, "string")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_STRING;
    } else if (zend_string_equals_literal_ci(value, "array")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_ARRAY;
    } else if (zend_string_equals_literal_ci(value, "object")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_OBJECT;
    } else if (zend_string_equals_literal_ci(value, "resource")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_RESOURCE;
    } else if (zend_string_equals_literal_ci(value, "reference")) {
      *(sp_php_type *)retval = SP_PHP_TYPE_REFERENCE;
    } else {
      pefree(value, 1);
      sp_log_err("error",
                 "%s) is expecting a valid php type ('false', 'true',"
                 " 'array'. 'object', 'long', 'double', 'null', 'resource', "
                 "'reference', 'undef') on line %zu",
                 keyword, sp_line_no);
      return -1;
    }
    pefree(value, 1);
    return consumed;
  } else {
    return -1;
  }
}

int parse_str(char *restrict line, char *restrict keyword, void *retval) {
  zend_string *value = NULL;

  size_t consumed = 0;
  value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  if (value) {
    *(zend_string **)retval = value;
    return consumed;
  }
  return -1;
}

int parse_cidr(char *restrict line, char *restrict keyword, void *retval) {
  size_t consumed = 0;
  zend_string *value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  sp_cidr *cidr = pecalloc(sizeof(sp_cidr), 1, 1);

  if (value) {
    if (-1 == get_ip_and_cidr(ZSTR_VAL(value), cidr)) {
      return -1;
    }
    *(sp_cidr **)retval = cidr;
    return consumed;
  } else {
    sp_log_err("config", "%s doesn't contain a valid cidr on line %zu", line,
               sp_line_no);
    return -1;
  }
}

int parse_regexp(char *restrict line, char *restrict keyword, void *retval) {
  /* TODO: Do we want to use pcre_study?
   * (http://www.pcre.org/original/doc/html/pcre_study.html)
   * maybe not: http://sljit.sourceforge.net/pcre.html*/
  size_t consumed = 0;
  zend_string *value = get_param(&consumed, line, SP_TYPE_STR, keyword);

  if (value) {
    sp_pcre *compiled_re = sp_pcre_compile(ZSTR_VAL(value));
    if (NULL != compiled_re) {
      *(sp_pcre **)retval = compiled_re;
      return consumed;
    }
  }
  char *closing_paren = strchr(line, ')');
  if (NULL != closing_paren) {
    closing_paren[0] = '\0';
  }
  sp_log_err("config",
             "'%s)' is expecting a valid regexp, and not '%s' on line %zu",
             keyword, line, sp_line_no);
  return -1;
}

int sp_parse_config(const char *conf_file) {
  FILE *fd = fopen(conf_file, "r");
  char *lineptr = NULL;
  size_t n = 0;
  sp_line_no = 1;

  if (fd == NULL) {
    sp_log_err("config", "Could not open configuration file %s : %s", conf_file,
               strerror(errno));
    return FAILURE;
  }

  while (getline(&lineptr, &n, fd) > 0) {
    /* We trash the terminal `\n`. This simplify the display of logs. */
    if (lineptr[strlen(lineptr) - 1] == '\n') {
      if (strlen(lineptr) >= 2 && lineptr[strlen(lineptr) - 2] == '\r') {
        lineptr[strlen(lineptr) - 2] = '\0';
      } else {
        lineptr[strlen(lineptr) - 1] = '\0';
      }
    }
    if (parse_line(lineptr) == -1) {
      fclose(fd);
      free(lineptr);
      return FAILURE;
    }
    free(lineptr);
    lineptr = NULL;
    n = 0;
    sp_line_no++;
  }
  fclose(fd);
  return SUCCESS;
}

void sp_disabled_function_list_free(sp_list_node *list) {
  sp_list_node *cursor = list;
  while (cursor) {
    sp_disabled_function *df = cursor->data;
    if (df && df->functions_list) sp_list_free(df->functions_list);
    if (df) {
      sp_tree_free(df->param);
      sp_tree_free(df->var);
    }
    cursor = cursor->next;
  }
}
