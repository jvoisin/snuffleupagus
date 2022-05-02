#include "php_snuffleupagus.h"


sp_list_node *parse_functions_list(const char *const value) {
  static const char *const sep = ">";

  if (NULL == strchr(value, sep[0])) {
    return NULL;
  }

  sp_list_node *list = NULL;
  char *tmp = strdup(value);
  const char *function_name;
  char *next_token = tmp;
  while ((function_name = strtok_r(NULL, sep, &next_token))) {
    list = sp_list_prepend(list, strdup(function_name));
  }
  free(tmp);

  return list;
}
