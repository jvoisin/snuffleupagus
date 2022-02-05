#include "php_snuffleupagus.h"

void sp_list_free(sp_list_node *node, void (*free_data_func)(void *data)) {
  while (node) {
    sp_list_node *tmp = node->next;
    if (free_data_func && node->data) {
      free_data_func(node->data);
    }
    pefree(node, 1);
    node = tmp;
  }
}

void sp_list_free2(sp_list_node *node) {
  while (node) {
    sp_list_node *tmp = node->next;
    if (node->data) {
      pefree(node->data, 1);
    }
    pefree(node, 1);
    node = tmp;
  }
}

// Thanks to https://en.wikipedia.org/wiki/Insertion_sort :>
sp_list_node *sp_list_sort(sp_list_node *pList,
                           int (*cmp_func)(sp_list_node const *const,
                                           sp_list_node const *const)) {
  sp_list_node *head = NULL;

  if (pList == NULL || pList->next == NULL) {
    return pList;
  }
  while (pList != NULL) {
    sp_list_node *current = pList;
    pList = pList->next;
    if (head == NULL || 0 > cmp_func(current, head)) {
      current->next = head;
      head = current;
    } else {
      sp_list_node *p = head;
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

sp_list_node *sp_list_insert(sp_list_node *list, void *data) {
  sp_list_node *new = pecalloc(sizeof(*new), 1, 1);
  sp_list_node *origin = list;
  new->data = data;
  new->next = NULL;

  if (list == NULL) {
    origin = new;
  } else {
    while (list->next) {
      list = list->next;
    }
    list->next = new;
  }
  return origin;
}

sp_list_node *sp_list_prepend(sp_list_node *list, void *data) {
  sp_list_node *new = pecalloc(sizeof(*new), 1, 1);
  new->next = list;
  new->data = data;
  return new;
}

size_t sp_list_len(sp_list_node *p) {
  size_t num = 0;
  for (; p; p = p->next) { num++; }
  return num;
}
