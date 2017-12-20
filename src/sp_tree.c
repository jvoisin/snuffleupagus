#include "php_snuffleupagus.h"

void sp_tree_free(sp_tree *tree) {
  while (tree) {
    sp_tree *tmp;
    pefree(tree->value, 1);
    sp_tree_free(tree->idx);
    tmp = tree;
    tree = tree->next;
    pefree(tmp, 1);
  }
}

sp_tree *sp_tree_new() {
  sp_tree *new = pecalloc(sizeof(sp_tree), 1, 1);
  new->next = new->idx = NULL;
  new->value = NULL;
  new->type = UNDEFINED;
  return new;
}
