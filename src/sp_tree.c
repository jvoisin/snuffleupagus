#include "php_snuffleupagus.h"

void free_sp_tree(sp_tree *sapin) {
  while (sapin) {
    sp_tree *tmp;
    pefree(sapin->value, 1);
    free_sp_tree(sapin->idx);
    tmp = sapin;
    sapin = sapin->next;
    pefree(tmp, 1);
  }
}

sp_tree *sp_tree_new() {
  sp_tree *new = pecalloc(sizeof(sp_tree), 1, 1);
  new->next = new->idx = NULL;
  new->value = NULL;
  new->type = 0;
  return new;
}
