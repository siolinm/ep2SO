#include "linkedList.h"

#include <stdio.h>
#include <stdlib.h>

llist *initList() {
  llist *l;
  l = (llist *) malloc(sizeof(llist));
  l->head = (lnode *) malloc(sizeof(lnode));
  l->tail = l->head;
  l->head->next = NULL;
  l->size = 0;

  return l;
}

llist *push(llist *l, int v) {
  lnode *newNode;
  newNode = (lnode *) malloc(sizeof(lnode));

  l->tail->next = newNode;
  newNode->next = NULL;
  newNode->value = v;
  l->tail = newNode;
  l->size++;

  return l;
}

void print(llist *l) {
  lnode *ln = l->head->next;
  int count = 1;

  fprintf(stderr, "[\n");
  while (ln) {
    fprintf(stderr, "\t%dº: %d\n", count++, ln->value);
    ln = ln->next;
  }
  fprintf(stderr, "]\n");
}

void freeList(llist *l) {
  lnode *aux;
  aux = l->head;
  while (aux) {
    l->head = l->head->next;
    free(aux);
    aux = l->head;
  }
  free(l);
}
