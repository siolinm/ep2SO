#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

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

void setPosicao(int PID, int i, int j) {
	metro(PID) = i;
	faixa(PID) = j;
}

void setvPosicao(int PID, int pos[2]) {
	metro(PID) = pos[0];
	faixa(PID) = pos[1];
}

Bool chegouChegada(int PID) {
	double randValue;
	Bool quebrouOuEliminado = False;

	volta_atual(PID) += 1;
	/* ver se quebrou? */
	if (volta_atual(PID) != 0 && n_cur > 5 && volta_atual(PID) % 6 == 0) {
		randValue = (rand() / RAND_MAX);
		if (randValue < 0.05) { /* quebrou */
			/* printo na tela */
			quebrouOuEliminado = True;
		}
	}

	/* ver se foi eliminado? */

	/* mudar a minha velocidade */

	return quebrouOuEliminado;
}

/** Função que executada por cada thread */
void *ciclista(void *argv) {
	int PID = *((int *)argv);
	free(argv);

	int round;
	int max_round;
	Bool fuiEliminado = False;

	int i, j; // Minha posição na pista
	i = metro(PID);
	j = faixa(PID);

	int f[2]; // posição da frente

	round = 0;
	max_round = 180 / velocidade(i);

	/* Esperando pela largada */
	pthread_barrier_wait(&barreira);

	while (!fuiEliminado) {
		if (round == 0) {
			i = metro(PID);
			j = faixa(PID);
			max_round = 180 / velocidade(i);

			pthread_mutex_lock(&mutex_pista[i][j]);
			f[0] = (i + 1) % d;
			f[1] = j;
			pthread_mutex_lock(&mutex_pista[f[0]][f[1]]);

			if (vpista(f) == 0) { /*  frente livre */
				pista(i, j) = 0;
				vpista(f) = PID + 1;
				setvPosicao(PID, f);
				/* chegou na chegada? */
				if (metro(PID) == 0) fuiEliminado = chegouChegada(PID);
			}

			pthread_mutex_unlock(&mutex_pista[f[0]][f[1]]);
			pthread_mutex_unlock(&mutex_pista[i][j]);

			/* trabalho */
			if (fuiEliminado) { /* foi eliminado */
				pthread_mutex_lock(&mutex_eliminado);
				n_cur--;
				ciclistaEliminado = True;
				pthread_mutex_unlock(&mutex_eliminado);
			}
		}

		/* espera */
		round = (round + 1) % max_round;
		pthread_barrier_wait(&barreira);
		pthread_mutex_lock(&mutex_barreira);
		if (ciclistaEliminado) {
			pthread_barrier_destroy(&barreira);
			pthread_barrier_init(&barreira, NULL, n_cur);
			ciclistaEliminado = False;
		}
		pthread_mutex_unlock(&mutex_barreira);
		pthread_barrier_wait(&barreira);
	}

	return NULL;
}

void mostreUso() {
	fprintf(stderr, "Faltando parâmetros, para usar digite:\n"
	                "./ep2 <d> <n>\n");
}

int main(int argc, char *argv[]) {
	if (argc < 3) mostreUso();

	srand(time(NULL));

	d = atoi(argv[1]);
	n = atoi(argv[2]);

	n_cur = n;
	pthread_barrier_init(&barreira, NULL, n_cur);

	pthread_mutex_init(&mutex_barreira, NULL);
	pthread_mutex_init(&mutex_eliminado, NULL);

	for (int i = 0; i < d; ++i)
		for (int j = 0; j < FAIXAS; ++j) {
			pista[i][j] = 0;
			pthread_mutex_init(&mutex_pista[i][j], NULL);
		}

	int *temp;
	for (int i = 0; i < n; i++) {
		temp = malloc(sizeof(int));
		*temp = i;

		velocidade(i) = 30;
		metro(i) = d - (i / (FAIXAS / 2)) - 1;
		faixa(i) = i % (FAIXAS / 2);
		pista[metro(i)][faixa(i)] = i + 1;
		volta_atual(i) = -1;
		quando_quebrou(i) = -1;
		chegou(i) = -1;

		pthread_create(&threads[i], NULL, ciclista, (void *)temp);
	}

	for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);

	return 0;
}
