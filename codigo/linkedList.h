#ifndef linked_list
#define linked_list
#include "util.h"

llist *initList();
llist *push(llist *l, int v);
void pop(llist * l);
int getLast(llist * l);
int getSize(llist *l);
void print(llist *l);
void freeList(llist *l);
void update(llist * l);

#endif /* ifndef linked_list */
