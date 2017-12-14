#include "php_snuffleupagus.h"

void free_arbre_du_ghetto(arbre_du_ghetto *sapin) {
  while (sapin) {
    arbre_du_ghetto *tmp;
    pefree(sapin->value, 1);
    free_arbre_du_ghetto(sapin->idx);
    tmp = sapin;
    sapin = sapin->next;
    pefree(tmp, 1);
  }
}

arbre_du_ghetto *arbre_du_ghetto_new() {
  arbre_du_ghetto *new = pecalloc(sizeof(arbre_du_ghetto), 1, 1);
  new->next = new->idx = NULL;
  new->value = NULL;
  new->type = 0;
  return new;
}
