#ifndef linked_list
#define linked_list


typedef struct node {
  int value;
  struct node *next;
} lnode;

typedef struct {
  lnode *head, *tail;
  int size;
} llist;

llist *initList();
llist *push(llist *l, int v);
void print(llist *l);
void freeList(llist *l);

#endif /* ifndef linked_list */
