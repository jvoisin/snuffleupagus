#ifndef SP_VAR_PARSER_H
#define SP_VAR_PARSER_H
#include "php_snuffleupagus.h"

typedef struct sp_token_s {
  elem_type type;
  char *token;
  unsigned int pos;
} sp_token_t;

zval *get_value(zend_execute_data *, const sp_tree *, bool);
sp_tree *parse_var(const char *);

#define OBJECT_TOKEN "->"
#define ARRAY_TOKEN "["
#define ARRAY_END_TOKEN "]"
#define STRING_TOKEN "\""
#define ESC_STRING_TOKEN "\'"
#define CLASS_TOKEN "::"

#define VARIABLE_TOKEN '$'

#define PRIVATE_PROP_FMT "%c%s%c%s"
#define PROTECTED_PROP_FMT "%c*%c%s"

#define REGEXP_VAR "^\\$[a-z_][a-z0-9_]*$"
#define REGEXP_CONST "^[a-z_0-9\\\\]*$"

#endif
