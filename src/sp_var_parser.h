#ifndef SP_VAR_PARSER_H
# define SP_VAR_PARSER_H
# include "php_snuffleupagus.h"
# include "sp_list.h"

typedef enum {
  object = 1,
  array = 2,
  litteral = 3,
  array_end = 4,
  string_delimiter = 5,
  namespace = 6,
  class = 7,
  var = 8,
  esc_string_delimiter = 9
} elem_type;

typedef struct sp_token_s {
  elem_type type;
  char *token;
  unsigned int pos;
} sp_token_t;

/*TODO: rename this*/
typedef struct parser_s {
  elem_type type;
  char *value;
  struct parser_s *idx;
  struct parser_s *next;
} arbre_du_ghetto;

char *get_value(zend_execute_data *, const arbre_du_ghetto *);
arbre_du_ghetto *parse_var(const char *);
void print_type_list(const char *, arbre_du_ghetto*, int);
void free_arbre_du_ghetto(arbre_du_ghetto *);

# define OBJECT_TOKEN "->"
# define ARRAY_TOKEN "["
# define ARRAY_END_TOKEN "]"
# define STRING_TOKEN "\""
# define ESC_STRING_TOKEN "\'"
# define NAMESPACE_TOKEN "\\"
# define CLASS_TOKEN "::"

# define VARIABLE_TOKEN '$'

# define PRIVATE_PROP_FMT "%c%s%c%s"
# define PROTECTED_PROP_FMT "%c*%c%s"

#endif
