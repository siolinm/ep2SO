#ifndef _UTIL_H
#define _UTIL_H

#include <pthread.h>

typedef enum { False, True } Bool;

#define D_MAX 2000
#define MAX_CICLISTAS 5 * D_MAX
#define FAIXAS 10
#define TRECHO 20

// True se queremos estamos no modo debug
#define debugOn True

typedef struct {
  int velocidade;     // velocidade atual
  int pos[2];         // posição na pista
  int quando_quebrou; // -1 caso não quebrou, t caso chegou no instante t
  int volta_atual;    // o número da volta atual
  int chegou;         // -1 caso não chegou ou t caso chegou no instante t
  int round;          // round atual
  int max_round;      // round máximo
  Bool terminou_acao; // se está na barreira de sincronização
  int eliminado;     // se precisa ser eliminado na linha de chegada
} competidor;

typedef struct node {
  int value;
  struct node *next;
  struct node *prev;
} lnode;

typedef struct {
  lnode *head;
  int size;
} llist;

competidor ciclistas[MAX_CICLISTAS];        // Array com todos os ciclistas
pthread_t threads[MAX_CICLISTAS];           // A thread de cada ciclista
int pista[D_MAX][FAIXAS];                   // Uma matriz de referência
pthread_mutex_t mutex_pista[D_MAX][FAIXAS]; // Um mutex para cada posição

pthread_barrier_t barreira;     // A barreira do pthread
pthread_mutex_t mutex_barreira; // Mutex de acesso à barreira

Bool ciclistaEliminado;          // Marca que algum ciclista foi eliminado
llist *ranking[MAX_CICLISTAS];   // Vetor com as classificações por volta
int quebraram[MAX_CICLISTAS];    // Vetor de ciclistas quebrados
int ult;                         // A última volta em que alguém foi eliminado
pthread_mutex_t mutex_eliminado; // Mutex de proteção

Bool primeiro_a_chegar; // Se False, alguém já passou pela região de sincronia

#define velocidade(i) ciclistas[i].velocidade
#define quando_quebrou(i) ciclistas[i].quando_quebrou
#define volta_atual(i) ciclistas[i].volta_atual
#define metro(i) ciclistas[i].pos[0]
#define faixa(i) ciclistas[i].pos[1]
#define chegou(i) ciclistas[i].chegou
#define round(i) ciclistas[i].round
#define max_round(i) ciclistas[i].max_round
#define terminou_acao(i) ciclistas[i].terminou_acao
#define eliminado(i) ciclistas[i].eliminado

#define pista(i, j) pista[i][j]
#define vpista(f) pista[f[0]][f[1]]

int d, n;      // Valores d e n do enunciado
int n_cur;     // Quantos ciclistas ainda estão correndo
int t_cur;     // milissegundos do tempo atual
int t_sec_cur; // O tempo atual em segundos

void setPosicao(int PID, int i, int j);
void setvPosicao(int PID, int pos[2]);
void debugar();

#endif /* ifndef _UTIL_H */
