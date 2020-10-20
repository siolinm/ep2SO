#ifndef _UTIL_H
#define _UTIL_H

#include <pthread.h>

typedef enum { False, True } Bool;

#define MAX_CICLISTAS 500
#define D_MAX 10000
#define FAIXAS 10

typedef struct {
	int velocidade;     // velocidade atual
	int pos[2];         // posição na pista
	int quando_quebrou; // -1 caso não quebrou, t caso chegou no instante t
	int volta_atual;    // o número da volta atual
	int chegou;         // -1 caso não chegou ou t caso chegou no instante t
} competidor;

competidor ciclistas[MAX_CICLISTAS];        // Array com todos os ciclistas
pthread_t threads[MAX_CICLISTAS];           // A thread de cada ciclista
int pista[D_MAX][FAIXAS];                   // Uma matriz de referência
pthread_mutex_t mutex_pista[D_MAX][FAIXAS]; // Um mutex para cada posição da matriz

pthread_barrier_t barreira;     // A barreira do pthread
pthread_mutex_t mutex_barreira; // Mutex de acesso à barreira

Bool ciclistaEliminado;          // Marca que algum ciclista foi elimitado
pthread_mutex_t mutex_eliminado; // Mutex de proteção

#define velocidade(i) ciclistas[i].velocidade
#define quando_quebrou(i) ciclistas[i].quando_quebrou
#define volta_atual(i) ciclistas[i].volta_atual
#define metro(i) ciclistas[i].pos[0]
#define faixa(i) ciclistas[i].pos[1]
#define chegou(i) ciclistas[i].chegou

#define pista(i, j) pista[i][j]
#define vpista(f) pista[f[0]][f[1]]

int d, n; // Valores d e n do enunciado
int n_cur;

void setPosicao(int PID, int i, int j);
void setvPosicao(int PID, int pos[2]);

#endif /* ifndef _UTIL_H */
