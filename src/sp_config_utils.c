#include "php_snuffleupagus.h"

size_t sp_line_no;

int parse_keywords(sp_config_functions *funcs, char *line) {
  int value_len = 0;
  const char *original_line = line;
  for (size_t i = 0; funcs[i].func; i++) {
    if (!strncmp(funcs[i].token, line, strlen(funcs[i].token))) {
      line += strlen(funcs[i].token);
      value_len = funcs[i].func(line, funcs[i].token, funcs[i].retval) + 1;
      if (value_len == 0) {  // bad parameter
        return -1;
      }
      line += value_len;
      i = -1;  // we start the loop again
    }
  }
  while (*line == ';' || *line == '\t' || *line == ' ') {
    line++;
  }

  if (*line == '#') {
    return 0;
  }

  if (*line) {
    sp_log_err("config", "Trailing chars '%s' at the end of '%s' on line %zu.",
               line, original_line, sp_line_no);
    return -1;
  }
  return 0;
}

char *get_param(size_t *consumed, char *restrict line, sp_type type,
                const char *restrict keyword) {
  enum { IN_ESCAPE, NONE } state = NONE;
  char *original_line = line;
  size_t j = 0;

  char *ret = NULL;
  if (NULL == line || '\0' == *line) {
    goto err;
  }

  ret = pecalloc(sizeof(char), strlen(original_line) + 1, 1);

  /* The first char of a string is always '"', since they MUST be quoted. */
  if ('"' == *line) {
    line++;
  } else {
    goto err;
  }

  for (size_t i = 0; line[i] && j < strlen(original_line) - 2; i++) {
    switch (line[i]) {
      case '"':
        /* A double quote at this point is either:
          - at the very end of the string.
          - escaped
          */
        if ((state == NONE) && (line[i + 1] == SP_TOKEN_END_PARAM)) {
          /* The `+2` if for
           1. the terminal double-quote
           2. the SP_TOKEN_END_PARAM
           */
          *consumed = i + 2;
          return ret;
        } else if (state == IN_ESCAPE) {
          break;  // we're on an escped double quote
        } else {
          goto err;
        }
      case '\\':
        if (state == NONE) {
          state = IN_ESCAPE;
          continue;
        }
      default:
        break;
    }
    if (state == IN_ESCAPE) {
      state = NONE;
    }
    ret[j++] = line[i];
  }
err:
  if (0 == j) {
    sp_log_err("error", "A valid string as parameter is expected on line %zu.",
               sp_line_no);
  } else {
    sp_log_err("error",
               "There is an issue with the parsing of '%s': it doesn't look "
               "like a valid string on line %zu.",
               original_line ? original_line : "NULL", sp_line_no);
  }
  line = NULL;
  return NULL;
}

zend_always_inline sp_list_node *parse_functions_list(char *value) {
  static const char *sep = ">";

  if (NULL == strchr(value, sep[0])) {
    return NULL;
  }

  sp_list_node *list = NULL;
  char *tmp = strdup(value);
  char *function_name;
  char *next_token = tmp;
  while ((function_name = strtok_r(NULL, sep, &next_token))) {
    list = sp_list_prepend(list, strdup(function_name));
  }
  free(tmp);

  return list;
}
