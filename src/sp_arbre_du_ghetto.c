#include "php_snuffleupagus.h"

void free_arbre_du_ghetto(arbre_du_ghetto *sapin) {
  while (sapin) {
    arbre_du_ghetto *tmp;
    free(sapin->value);
    free_arbre_du_ghetto(sapin->idx);
    tmp = sapin;
    sapin = sapin->next;
    free(tmp);
  }
}

arbre_du_ghetto *arbre_du_ghetto_new() {
  arbre_du_ghetto *new = malloc(sizeof(arbre_du_ghetto));
  new->next = new->idx = NULL;
  new->value = NULL;
  new->type = 0;
  return new;
}
