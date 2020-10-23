#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"

Bool chegouChegada(int PID) {
    double randValue;
    Bool quebrouOuEliminado = False;

    printf("PID %d passou na chegada\n", PID);

    volta_atual(PID) += 1;

    if (volta_atual(PID) == 0) return False;

    /* ver se quebrou? */
    if (volta_atual(PID) != 0 && n_cur > 5 && volta_atual(PID) % 6 == 0) {
        randValue = (((double) rand()) / RAND_MAX);
        if (randValue < 0.05) { /* quebrou */
            /* printo na tela */
            quebrouOuEliminado = True;
        }
    }

    /* ver se foi eliminado? */
    /* pessoasNaVolta[volta_atual(PID)]++; */
    /* if (pessoasNaVolta[volta_atual(PID)+1] == n_cur -1) */
    /* quebrouOuEliminado = True; */

    /* mudar a minha velocidade */
    randValue = (((double) rand()) / RAND_MAX);
    if (velocidade(PID) == 30 && randValue < 0.8) {
        printf("%d mudou de velocidade: 30 -> 60\n", PID);
        velocidade(PID) = 60;
    } else if (velocidade(PID) == 60 && randValue < 0.4) {
        printf("%d mudou de velocidade: 60 -> 30\n", PID);
        velocidade(PID) = 30;
    }

    // perguntar no forum se é isso ou se é 10% de ter um a 90
    if (n_cur <= 2 && randValue < 0.1) velocidade(PID) = 90;

    return quebrouOuEliminado;
}

/** Função que executada por cada thread */
void *ciclista(void *argv) {
    int PID = *((int *) argv);
    free(argv);

    int round;
    int max_round;
    Bool fuiEliminado = False;
    Bool andei = False;

    int i, j; // Minha posição na pista
    i = metro(PID);
    j = faixa(PID);

    int f[2]; // posição da frente

    // k = pode ultrapassar pela faixa k
    int k;

    round = 0;
    max_round = 180 / velocidade(PID);
    primeiro_a_chegar = True;

    /* Esperando pela largada */
    /* sleep(rand() % 10); */
    /* printf("PID = %d pronto\n", PID); */
    pthread_barrier_wait(&barreira);
    /* printf("Corrida começada!\n"); */

    while (!fuiEliminado) {
        if (round == 0) {
            i = metro(PID);
            j = faixa(PID);
            andei = False;

            pthread_mutex_lock(&mutex_pista[i][j]);
            f[0] = (i + 1) % d;
            f[1] = j;
            pthread_mutex_lock(&mutex_pista[f[0]][f[1]]);

            /* TODO: 30km não pode ultrapassar <22-10-20, Lucas> */
            if (vpista(f) == 0) { /*  frente livre */
                pista(i, j) = 0;
                vpista(f) = PID + 1;
                setvPosicao(PID, f);
                andei = True;
                /* chegou na chegada? */
                if (metro(PID) == 0) fuiEliminado = chegouChegada(PID);
            }
            pthread_mutex_unlock(&mutex_pista[f[0]][f[1]]);

            if (!andei) { /* tento ultrapassar */
                for (k = j + 1; k < FAIXAS && !andei; ++k) {
                    pthread_mutex_lock(&mutex_pista[i][k]);
                    if (pista[i][k] == 0) {
                        pthread_mutex_lock(&mutex_pista[i + 1][k]);
                        if (pista[i + 1][k] == 0) {
                            /* então, posso ultrapassar */
                            pista(i, j) = 0;
                            pista(i + 1, k) = PID + 1;
                            setPosicao(PID, i + 1, k);
                            andei = True;
                            /* chegou na chegada? */
                            if (metro(PID) == 0)
                                fuiEliminado = chegouChegada(PID);
                        }
                        pthread_mutex_unlock(&mutex_pista[i + 1][k]);
                    }
                    pthread_mutex_unlock(&mutex_pista[i][k]);
                }
            }

            pthread_mutex_unlock(&mutex_pista[i][j]);

            if (fuiEliminado) { /* foi eliminado */
                pthread_mutex_lock(&mutex_eliminado);
                n_cur--;
                ciclistaEliminado = True;
                pthread_mutex_unlock(&mutex_eliminado);
            }
            max_round = 180 / velocidade(PID);
        }

        round = (round + 1) % max_round;
        /* if (debugOn) printf("Eu %d terminei meu trabalho\n", PID); */
        pthread_barrier_wait(&barreira);
        pthread_mutex_lock(&mutex_barreira);

        if (primeiro_a_chegar) {
            /* if (debugOn) printf("Primeiro dentro da barreira = %d\n", PID);
             */
            primeiro_a_chegar = False;
            t_cur += 20;
            usleep(20 * 1000);

            if (ciclistaEliminado) {
                pthread_barrier_destroy(&barreira);
                pthread_barrier_init(&barreira, NULL, n_cur);
                ciclistaEliminado = False;
            }

            if (debugOn &&
                ((t_cur % 60 == 0) || (n_cur <= 2 && t_cur % 20 == 0)))
                debugar();
        }

        pthread_mutex_unlock(&mutex_barreira);
        pthread_barrier_wait(&barreira);
        primeiro_a_chegar = True;
        /* if (debugOn) printf("Eu %d comecei meu trabalho\n", PID); */
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

    /* srand(time(NULL)); */
    srand(0);

    d = atoi(argv[1]);
    n = atoi(argv[2]);

    n_cur = n;
    t_cur = 0;
    pthread_barrier_init(&barreira, NULL, n_cur);

    pthread_mutex_init(&mutex_barreira, NULL);
    pthread_mutex_init(&mutex_eliminado, NULL);

    for (int i = 0; i < d; ++i)
        for (int j = 0; j < FAIXAS; ++j) {
            pista[i][j] = 0;
            pthread_mutex_init(&mutex_pista[i][j], NULL);
        }

    /* condições iniciais dos ciclistas */
    for (int i = 0; i < n; i++) {
        metro(i) = d - (i / (FAIXAS / 2)) - 1;
        faixa(i) = i % (FAIXAS / 2) + ((i / (FAIXAS / 2)) % (FAIXAS / 2));
        pista[metro(i)][faixa(i)] = i + 1;

        velocidade(i) = 30;
        volta_atual(i) = -1;
        quando_quebrou(i) = -1;
        chegou(i) = -1;
    }

    if (debugOn) debugar();

    int *temp;
    for (int i = 0; i < n; i++) {
        temp = malloc(sizeof(int));
        *temp = i;
        printf("Criando %d com velocidade %d\n", i, velocidade(i));
        pthread_create(&threads[i], NULL, ciclista, (void *) temp);
    }

    for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mutex_barreira);
    pthread_mutex_destroy(&mutex_eliminado);

    for (int i = 0; i < d; ++i)
        for (int j = 0; j < FAIXAS; ++j) {
            pthread_mutex_destroy(&mutex_pista[i][j]);
        }

    pthread_barrier_destroy(&barreira);

    return 0;
}
