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

sp_node_t *sp_list_new() {
  sp_node_t *new = pecalloc(sizeof(*new), 1, 1);
  new->next = new->data = new->head = NULL;
  return new;
}

// Thanks to https://en.wikipedia.org/wiki/Insertion_sort :>
sp_node_t *sp_list_sort(sp_node_t *pList, int (*cmp_func)(sp_node_t *, sp_node_t *)) {
  sp_node_t *head = NULL;

  if (pList == NULL || pList->next == NULL) {
    return pList;
  }
  while (pList != NULL) {
    sp_node_t *current = pList;
    pList = pList->next;
    if (head == NULL || 0 > cmp_func(current, head)) {
      current->next = head;
      head = current;
    } else {
      sp_node_t *p = head;
      while (p != NULL) {
	if (p->next == NULL || 0 > cmp_func(current, p->next)) {
	  current->next = p->next;
	  p->next = current;
	  break;
	}
	p = p->next;
      }
    }
  }
  return head;
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
