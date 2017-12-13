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
} arbre_du_ghetto;

zval *get_value(zend_execute_data *, const arbre_du_ghetto *, bool);
arbre_du_ghetto *arbre_du_ghetto_new();
arbre_du_ghetto *parse_var(const char * restrict);
void print_type_list(const char *, arbre_du_ghetto*, int);
void free_arbre_du_ghetto(arbre_du_ghetto *);

# define OBJECT_TOKEN "->"
# define ARRAY_TOKEN "["
# define ARRAY_END_TOKEN "]"
# define STRING_TOKEN "\""
# define ESC_STRING_TOKEN "\'"
# define CLASS_TOKEN "::"

# define VARIABLE_TOKEN '$'

# define PRIVATE_PROP_FMT "%c%s%c%s"
# define PROTECTED_PROP_FMT "%c*%c%s"

#endif
