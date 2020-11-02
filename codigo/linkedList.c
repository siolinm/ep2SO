#include "linkedList.h"

#include <stdio.h>
#include <stdlib.h>

llist *initList() {
  llist *l;
  l = (llist *) malloc(sizeof(llist));
  l->head = (lnode *) malloc(sizeof(lnode));
  l->head->next = l->head->prev = l->head;
  l->size = 0;

  return l;
}

llist *push(llist *l, int v) {
  lnode *newNode;
  newNode = (lnode *) malloc(sizeof(lnode));
  newNode->value = v;
  newNode->next = l->head;
  newNode->prev = l->head->prev;

  newNode->prev->next = newNode;
  l->head->prev = newNode;
  l->size++;

  return l;
}

Bool isEmpty(llist * l){
  return (l->size == 0);
}

int getSize(llist *l) {
  return (l == NULL ? 0 : l->size);
}

void pop(llist * l){
  lnode * aux;
  aux = l->head->prev;
  l->head->prev = aux->prev;
  l->head->prev->next = l->head;
  l->size--;
  free(aux);
}

void update(llist * l){
  lnode * aux, * prev, *next;
  aux = l->head->next;
  next = aux->next;
  prev = aux->prev;

  while(aux != l->head){
    if(eliminado(aux->value)){
      next->prev = aux->prev;
      prev->next = next;
      l->size--;
      free(aux);
    }
    aux = next;
    next = aux->next;
    prev = aux->prev;
  }
}

int getLast(llist * l){
  return l->head->prev->value;
}

void print(llist *l) {
  lnode *ln = l->head->next;
  int count = 1;

  fprintf(stderr, "[\n");
  while (ln != l->head) {
    fprintf(stderr, "\t%dº: %d\n", count++, ln->value);
    ln = ln->next;
  }
  fprintf(stderr, "]\n");
}

void freeList(llist *l) {
  lnode *aux;
  aux = l->head;
  l->head->prev->next = NULL;
  while (aux) {
    l->head = l->head->next;
    free(aux);
    aux = l->head;
  }
  free(l);
}
