#include <errno.h>
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
    sp_log_err("config", "Invalid configuration prefix for '%s' on line %zu.",
               line, sp_line_no);
    return -1;
  }
  ptr += strlen(SP_TOKEN_BASE);

  for (size_t i = 0; sp_func[i].func; i++) {
    if (!strncmp(sp_func[i].token, ptr, strlen(sp_func[i].token))) {
      return sp_func[i].func(ptr + strlen(sp_func[i].token));
    }
  }
  sp_log_err("config", "Invalid configuration section '%s' on line %zu.", line,
             sp_line_no);
  return -1;
}

/* keyword parsing */
int parse_empty(char *restrict line, char *restrict keyword, void *retval) {
  *(bool *)retval = true;
  return 0;
}

int parse_php_type(char *restrict line, char *restrict keyword, void *retval) {
  size_t consumed = 0;
  char *value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  if (value) {
    if (0 == strcasecmp("undef", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_UNDEF;
    } else if (0 == strcasecmp("null", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_NULL;
    } else if (0 == strcasecmp("true", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_TRUE;
    } else if (0 == strcasecmp("false", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_FALSE;
    } else if (0 == strcasecmp("long", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_LONG;
    } else if (0 == strcasecmp("double", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_DOUBLE;
    } else if (0 == strcasecmp("string", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_STRING;
    } else if (0 == strcasecmp("array", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_ARRAY;
    } else if (0 == strcasecmp("object", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_OBJECT;
    } else if (0 == strcasecmp("resource", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_RESOURCE;
    } else if (0 == strcasecmp("reference", value)) {
      *(sp_php_type *)retval = SP_PHP_TYPE_REFERENCE;
    } else {
      pefree(value, 1);
      sp_log_err("error",
                 "%s) is expecting a valid php type ('false', 'true',"
                 " 'array'. 'object', 'long', 'double', 'null', 'resource', "
                 "'reference',"
                 " 'undef') on line %zu.",
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
  char *value = NULL;

  size_t consumed = 0;
  value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  if (value) {
    *(char **)retval = value;
    return consumed;
  }
  return -1;
}

int parse_cidr(char *restrict line, char *restrict keyword, void *retval) {
  size_t consumed = 0;
  char *value = get_param(&consumed, line, SP_TYPE_STR, keyword);
  sp_cidr *cidr = pecalloc(sizeof(sp_cidr), 1, 1);

  if (value) {
    if (-1 == get_ip_and_cidr(value, cidr)) {
      return -1;
    }
    *(sp_cidr **)retval = cidr;
    return consumed;
  } else {
    sp_log_err("config", "%s doesn't contain a valid cidr on line %zu.", line,
               sp_line_no);
    return -1;
  }
}

int parse_regexp(char *restrict line, char *restrict keyword, void *retval) {
  /* TODO: Do we want to use pcre_study?
   * (http://www.pcre.org/original/doc/html/pcre_study.html)
   * maybe not: http://sljit.sourceforge.net/pcre.html*/
  size_t consumed = 0;
  char *value = get_param(&consumed, line, SP_TYPE_STR, keyword);

  if (value) {
    const char *pcre_error;
    int pcre_error_offset;
    pcre *compiled_re = sp_pcre_compile(value, PCRE_CASELESS, &pcre_error,
                                        &pcre_error_offset, NULL);
    if (NULL == compiled_re) {
      sp_log_err("config", "Failed to compile '%s': %s on line %zu.", value,
                 pcre_error, sp_line_no);
    } else {
      *(pcre **)retval = compiled_re;
      return consumed;
    }
  }
  char *closing_paren = strchr(line, ')');
  if (NULL != closing_paren) {
    closing_paren[0] = '\0';
  }
  sp_log_err("config",
             "'%s)' is expecting a valid regexp, and not '%s' on line %zu.",
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
      if (lineptr[strlen(lineptr) - 2] == '\r') {
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
