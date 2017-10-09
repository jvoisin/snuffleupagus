#include "sp_list.h"
#include <stdio.h>
#include <stdlib.h>
#include "php_snuffleupagus.h"

void sp_list_free(sp_node_t *node) {
  while(node) {
    sp_node_t *tmp = node->next;
    pefree(node, 1);
    node = tmp;
  }
}

sp_node_t *sp_new_list() {
  sp_node_t *new = pecalloc(sizeof(*new), 1, 1);
  new->next = new->data = new->head = NULL;
  return new;
}

void sp_list_insert(sp_node_t *list, void *data) {
  if (list->head == NULL) {
    list->data = data;
    list->next = NULL;
    list->head = list;
  } else {
    sp_node_t *new = pecalloc(sizeof(*new), 1, 1);

    new->data = data;
    new->next = NULL;
    new->head = list;

    while (list->next) {
      list = list->next;
    }
    list->next = new;
  }
}

void sp_list_prepend(sp_node_t *list, void *data) {
  if (list->head == NULL) {
    list->data = data;
    list->next = NULL;
    list->head = list;
  } else {
    sp_node_t *new = pecalloc(sizeof(*new), 1, 1);

    new->next = list->next;
    list->next = new;

    new->head = list;

    new->data = list->data;
    list->data = data;
  }
}