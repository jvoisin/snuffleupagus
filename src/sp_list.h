#ifndef SP_LIST_H
#define SP_LIST_H

typedef struct sp_node_s {
  struct sp_node_s *next;
  void *data;

} sp_list_node;

sp_list_node *sp_list_sort(sp_list_node *,
                           int (*)(sp_list_node *, sp_list_node *));
sp_list_node *sp_list_insert(sp_list_node *, void *);
sp_list_node *sp_list_prepend(sp_list_node *, void *);
void sp_list_free(sp_list_node *);

#endif
