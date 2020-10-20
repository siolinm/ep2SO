#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

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

	/* printf("velocidade = %d\nposicao = %d %d\nvolta = %d\n", velocidade(PID), metro(PID), faixa(PID), */
		   /* volta_atual(PID)); */

	int round;
	int max_round;
	Bool fuiEliminado = False;

	int i, j; // Minha posição na pista
	i = metro(PID);
	j = faixa(PID);

	int f[2]; // posição da frente

	round = 0;
	max_round = 180 / velocidade(PID);

	/* Esperando pela largada */
	pthread_barrier_wait(&barreira);

	while (!fuiEliminado) {
		if (round == 0) {
			i = metro(PID);
			j = faixa(PID);
			max_round = 180 / velocidade(PID);

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
	if (argc < 3) {
		mostreUso();
		return -1;
	}

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

	for (int i = 0; i < n; i++) {
		ciclistas[i].velocidade = 30;
		metro(i) = d - (i / (FAIXAS / 2)) - 1;
		faixa(i) = i % (FAIXAS / 2);
		pista[metro(i)][faixa(i)] = i + 1;
		volta_atual(i) = -1;
		quando_quebrou(i) = -1;
		chegou(i) = -1;
	}

	int *temp;
	for (int i = 0; i < n; i++) {
		temp = malloc(sizeof(int));
		*temp = i;
		printf("Criando %d com velocidade %d\n", i, velocidade(i));
		pthread_create(&threads[i], NULL, ciclista, (void *)temp);
	}

	for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);

	return 0;
}
