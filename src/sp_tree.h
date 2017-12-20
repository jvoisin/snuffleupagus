#ifndef SP_TREE_H
#define SP_TREE_H
#include "php_snuffleupagus.h"

typedef enum {
  UNDEFINED = 0,
  OBJECT,
  ARRAY,
  ARRAY_END,
  STRING_DELIMITER,
  CLASS,
  VAR,
  ESC_STRING_DELIMITER,
  CONSTANT
} elem_type;

typedef struct parser_s {
  elem_type type;
  char *value;
  struct parser_s *idx;
  struct parser_s *next;
} sp_tree;

sp_tree *sp_tree_new();
void sp_tree_free(sp_tree *);

#endif
