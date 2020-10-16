#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#define MAX_CICLISTAS 500
#define D_MAX 10000
#define FAIXAS 10

typedef struct {
	int velocidade;     // velocidade atual
	int i, j;           // posição na pista
	int quando_quebrou; // -1 caso não quebrou, t caso chegou no instante t
	int volta_atual;    // o número da volta atual
	int chegou;         // -1 caso não chegou ou t caso chegou no instante t
} competidor;

competidor ciclistas[MAX_CICLISTAS]; // Array com todos os ciclistas
int n_ciclistas;                     // Número de ciclistas no array
int pista[D_MAX][FAIXAS];
pthread_mutex_t mutex[D_MAX][FAIXAS];

pthread_barrier_t barreira;

int d, n; // Valores d e n do enunciado

/** Função que executada por cada thread */
void *ciclista(void *arqv) {
	int round;
	int max_round;


	return NULL;
}

void mostreUso() {
	fprintf(stderr, "Faltando parâmetros, para usar digite:\n"
	                "./ep2 <d> <n>\n");
}

int main(int argc, char *argv[]) {
	if (argc < 3) mostreUso();

	d = atoi(argv[1]);
	n = atoi(argv[2]);

	return 0;
}
