#ifndef SP_LIST_H
#define SP_LIST_H

typedef struct sp_node_s {
  struct sp_node_s *next;
  struct sp_node_s *head;
  void *data;

} sp_node_t;

sp_node_t *sp_new_list();
void sp_list_insert(sp_node_t *, void *);
void sp_list_free(sp_node_t *);
void sp_list_prepend(sp_node_t *, void *);

#endif
