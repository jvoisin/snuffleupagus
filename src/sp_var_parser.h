#ifndef SP_VAR_PARSER_H
# define SP_VAR_PARSER_H
# include "php_snuffleupagus.h"
# include "sp_list.h"

typedef enum {
  OBJECT = 1,
  ARRAY,
  ARRAY_END,
  STRING_DELIMITER,
  CLASS,
  VAR,
  ESC_STRING_DELIMITER,
  CONSTANT
} elem_type;

typedef struct sp_token_s {
  elem_type type;
  char *token;
  unsigned int pos;
} sp_token_t;

typedef struct parser_s {
  elem_type type;
  char *value;
  struct parser_s *idx;
  struct parser_s *next;
} sp_tree;

zval *get_value(zend_execute_data *, const sp_tree *, bool);
sp_tree *sp_tree_new();
sp_tree *parse_var(const char *);
void print_type_list(const char *, sp_tree*, int);
void sp_tree_free(sp_tree *);

# define OBJECT_TOKEN "->"
# define ARRAY_TOKEN "["
# define ARRAY_END_TOKEN "]"
# define STRING_TOKEN "\""
# define ESC_STRING_TOKEN "\'"
# define CLASS_TOKEN "::"

# define VARIABLE_TOKEN '$'

# define PRIVATE_PROP_FMT "%c%s%c%s"
# define PROTECTED_PROP_FMT "%c*%c%s"

# define REGEXP_VAR "^\\$[a-z_][a-z0-9_]*$"
# define REGEXP_CONST "^[a-z_0-9\\\\]*$"

#endif
