#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"

Bool chegouChegada(int PID) {
  double randValue;
  Bool quebrouOuEliminado = False;

  fprintf(stderr, "PID %d passou na chegada\n", PID);

  volta_atual(PID) += 1;

  if (volta_atual(PID) == 0) return False;

  /* ver se quebrou? */
  if (volta_atual(PID) != 0 && n_cur > 5 && volta_atual(PID) % 3 == 0) {
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
    /* fprintf("%d mudou de velocidade: 30 -> 60\n", PID); */
    velocidade(PID) = 60;
  } else if (velocidade(PID) == 60 && randValue < 0.4) {
    /* fprintf(stderr, "%d mudou de velocidade: 60 -> 30\n", PID); */
    velocidade(PID) = 30;
  }

  // perguntar no forum se é isso ou se é 10% de ter um a 90
  if (n_cur <= 2 && randValue < 0.1) {
    /* fprintf(stderr, "%d mudou de velocidade para 90\n", PID); */
    velocidade(PID) = 90;
  }

  return quebrouOuEliminado;
}

void andarPara(int PID, int i, int j, Bool *andei, Bool *fuiEliminado) {
  int cur_i, cur_j;

  cur_i = metro(PID);
  cur_j = faixa(PID);

  if (pista(cur_i, cur_j) == PID + 1) pista(cur_i, cur_j) = 0;
  pista(i, j) = PID + 1;

  metro(PID) = i;
  faixa(PID) = j;

  *andei = True;

  /* chegou na chegada? */
  if (metro(PID) == 0) *fuiEliminado = chegouChegada(PID);
}

/** Função que executada por cada thread */
void *ciclista(void *argv) {
  int PID = *((int *) argv);
  free(argv);

  Bool fuiEliminado = False;
  Bool andei = False;

  int i, j; // Minha posição na pista
  i = metro(PID);
  j = faixa(PID);

  int f[2]; // posição da frente

  // k = pode ultrapassar pela faixa k
  int k;

  round(PID) = 0;
  max_round(PID) = 180 / velocidade(PID);
  primeiro_a_chegar = True;

  /* Esperando pela largada */
  /* sleep(rand() % 10); */
  /* fprintf(stderr, "PID = %d pronto\n", PID); */
  pthread_barrier_wait(&barreira);
  /* fprintf(stderr, "Corrida começada!\n"); */

  while (!fuiEliminado) {
    if (round(PID) == 0) {
      i = metro(PID);
      j = faixa(PID);
      andei = False;

      pthread_mutex_lock(&mutex_pista[i][j]);
      /* fprintf(stderr, "%d: Passei pela primeira\n", PID); */
      f[0] = (i + 1) % d;
      f[1] = j;
      pthread_mutex_lock(&mutex_pista[f[0]][f[1]]);
      /* fprintf(stderr, "%d: Passei pela segunda\n", PID); */

      while (vpista(f) != 0 && !terminou_acao(vpista(f) - 1)) {
        /* fprintf(stderr, "sou %d e estou esperando\n", PID); */
        pthread_mutex_unlock(&mutex_pista[f[0]][f[1]]);
        usleep(20000);
        /* fprintf(stderr, "Sou %d\n", PID); */
        /* getchar(); */
        pthread_mutex_lock(&mutex_pista[f[0]][f[1]]);
      }

      if (vpista(f) == 0) /*  frente livre */
        andarPara(PID, f[0], f[1], &andei, &fuiEliminado);

      pthread_mutex_unlock(&mutex_pista[f[0]][f[1]]);

      if (!andei && velocidade(PID) != 30) { /* tento ultrapassar */
        for (k = j + 1; k < FAIXAS && !andei; ++k) {
          if (pthread_mutex_trylock(&mutex_pista[i][k]) == 0) {
            if (pista[i][k] == 0)
              if (pthread_mutex_trylock(&mutex_pista[(i + 1) % d][k]) == 0) {
                if (pista[(i + 1) % d][k] == 0)
                  andarPara(PID, (i + 1) % d, k, &andei, &fuiEliminado);

                pthread_mutex_unlock(&mutex_pista[(i + 1) % d][k]);
              }

            pthread_mutex_unlock(&mutex_pista[i][k]);
          }
        }
      }

      if (fuiEliminado) { /* foi eliminado */
        pthread_mutex_lock(&mutex_eliminado);
        n_cur--;
        ciclistaEliminado = True;
        pista(metro(PID), faixa(PID)) = 0;
        // fprintf(stderr, "%d foi eliminado.\n", PID);
        /* getchar(); */
        pthread_mutex_unlock(&mutex_eliminado);
      }

      pthread_mutex_unlock(&mutex_pista[i][j]);
      max_round(PID) = 180 / velocidade(PID);
    }

    round(PID) = (round(PID) + 1) % max_round(PID);

    // if (debugOn) fprintf(stderr, "Eu %d terminei meu trabalho\n", PID);
    terminou_acao(PID) = True;

    pthread_barrier_wait(&barreira);
    if (fuiEliminado) break;
    pthread_mutex_lock(&mutex_barreira);

    /* usleep(2000); */

    terminou_acao(PID) = False;

    if (primeiro_a_chegar) {
      /* if (debugOn) fprintf(stderr, "Primeiro dentro da barreira = %d\n",
       * PID);
       */

      primeiro_a_chegar = False;

      if (ciclistaEliminado) {
        pthread_barrier_destroy(&barreira);
        pthread_barrier_init(&barreira, NULL, n_cur);
        ciclistaEliminado = False;
      }      


      if (debugOn && ((t_cur % 60 == 0) || (n_cur <= 2 && t_cur % 20 == 0))) {
        debugar();
        getchar();
        /* usleep(2000); */
      }

      // fprintf(stderr, "\n\n\n\n");
      // fprintf(stderr, "n_cur = %d\n", n_cur);

      t_cur += 20;
      /* usleep(20 * 1000); */
    }

    pthread_mutex_unlock(&mutex_barreira);
    pthread_barrier_wait(&barreira);
    primeiro_a_chegar = True;
    /* if (debugOn) fprintf(stderr, "Eu %d comecei meu trabalho\n", PID); */
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
  t_sec_cur = 0;
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

    faixa(i) = i % (FAIXAS / 2) + (i / (FAIXAS / 2) % 2) * (FAIXAS / 2);
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
    fprintf(stderr, "Criando %d com velocidade %d\n", i, velocidade(i));
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
