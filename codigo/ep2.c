#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "linkedList.h"
#include "util.h"

Bool chegouChegada(int PID) {
  double randValue;
  Bool quebrouOuEliminado = False;

  volta_atual(PID) += 1;

  if (volta_atual(PID) == 0) return False;

  pthread_mutex_lock(&mutex_eliminado);
  if (eliminado(PID)) {
    pthread_mutex_unlock(&mutex_eliminado);
    return True;
  }

  /* ver se foi eliminado? */
  if (volta_atual(PID) - 1 == ult && ranking[ult]) {
    if (ranking[ult]->size + quebraram[ult] == ranking[ult - 1]->size - 1) {
      eliminado(PID) = True;
      quebrouOuEliminado = True;
      ult += 2;

      if (debugOn) fprintf(stderr, "PID %d eliminado em t = %d.\n", PID, t_cur);
    }

    while (ranking[ult] &&
           ranking[ult]->size + quebraram[ult] == ranking[ult - 1]->size) {
      int i = ranking[ult]->tail->value;
      eliminado(i) = True; // aviso para quando ele passar pela chegada
      ult += 2;
    }
  }

  /* ver se quebrou? */
  if (!quebrouOuEliminado && n_cur > 5 && volta_atual(PID) % 6 == 0) {
    randValue = (((double) rand()) / RAND_MAX);
    if (randValue < 0.05) { /* quebrou */
      quebrouOuEliminado = True;
      quebraram[volta_atual(PID) - 1]++;

      if (debugOn) fprintf(stderr, "PID %d quebrou em t = %d.\n", PID, t_cur);
    }
  }
  pthread_mutex_unlock(&mutex_eliminado);

  /* mudar a minha velocidade */
  if (!quebrouOuEliminado) {
    randValue = (((double) rand()) / RAND_MAX);
    if (velocidade(PID) == 30 && randValue < 0.8)
      velocidade(PID) = 60;
    else if (velocidade(PID) == 60 && randValue < 0.4)
      velocidade(PID) = 30;

    pthread_mutex_lock(&mutex_eliminado);
    /* TODO: Perguntar no fórum se é isso ou se é 10% de ter um a 90 <30-10-20,
     * Lucas> */
    if (n_cur <= 2 && randValue < 0.1) velocidade(PID) = 90;
    pthread_mutex_unlock(&mutex_eliminado);
  }

  if (!quebrouOuEliminado) {
    pthread_mutex_lock(&mutex_eliminado);
    if (ranking[volta_atual(PID) - 1] == NULL) // primeiro a chegar
      ranking[volta_atual(PID) - 1] = initList();

    push(ranking[volta_atual(PID) - 1], PID);

    if (debugOn) {
      fprintf(stderr, "Volta %d : ", volta_atual(PID) - 1);
      print(ranking[volta_atual(PID) - 1]);
    }

    pthread_mutex_unlock(&mutex_eliminado);
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

/* Função que executada por cada thread */
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
  pthread_barrier_wait(&barreira);

  while (!fuiEliminado) {
    if (round(PID) == 0) {
      i = metro(PID);
      j = faixa(PID);
      andei = False;

      pthread_mutex_lock(&mutex_pista[i][j]);
      f[0] = (i + 1) % d;
      f[1] = j;
      pthread_mutex_lock(&mutex_pista[f[0]][f[1]]);

      while (vpista(f) != 0 && !terminou_acao(vpista(f) - 1)) {
        pthread_mutex_unlock(&mutex_pista[f[0]][f[1]]);
        usleep(200000);
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
        pthread_mutex_unlock(&mutex_eliminado);
      }

      pthread_mutex_unlock(&mutex_pista[i][j]);
      max_round(PID) = 180 / velocidade(PID);
    }

    round(PID) = (round(PID) + 1) % max_round(PID);

    terminou_acao(PID) = True;

    pthread_barrier_wait(&barreira);
    if (fuiEliminado) break;
    pthread_mutex_lock(&mutex_barreira);

    terminou_acao(PID) = False;
    if (n_cur == 1) fuiEliminado = True;

    if (primeiro_a_chegar) {
      primeiro_a_chegar = False;

      if (ciclistaEliminado) {
        pthread_barrier_destroy(&barreira);
        pthread_barrier_init(&barreira, NULL, n_cur);
        ciclistaEliminado = False;
      }

      if (debugOn && ((t_cur % 60 == 0) || (n_cur <= 2 && t_cur % 20 == 0))) {
        debugar();
      }

      t_cur += 20;
    }

    pthread_mutex_unlock(&mutex_barreira);
    pthread_barrier_wait(&barreira);
    primeiro_a_chegar = True;
  }

  return NULL;
}

void mostreUso() {
  fprintf(stderr, "Faltando parâmetros, para usar digite:\n"
                  "./ep2 d n <debug>,\n"
                  "onde debug indica que o modo debug será ligado caso 1.\n");
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    mostreUso();
    return -1;
  }

  /* Inicializando o rand */
  /* srand(time(NULL)); */
  srand(0);

  /* Pegando parâmetros da entrada */
  d = atoi(argv[1]);
  n = atoi(argv[2]);

  /* Definindo variáveis globais */
  n_cur = n;
  t_cur = 0;
  t_sec_cur = 0;
  ult = 1;
  debugOn = (argc > 3);
  for (int i = 0; i < 2 * n; i++) {
    ranking[i] = NULL;
    quebraram[i] = 0;
  }

  /* Inicializando mutexes e barreiras */
  pthread_barrier_init(&barreira, NULL, n_cur);

  pthread_mutex_init(&mutex_barreira, NULL);
  pthread_mutex_init(&mutex_eliminado, NULL);

  for (int i = 0; i < d; ++i)
    for (int j = 0; j < FAIXAS; ++j) {
      pista[i][j] = 0;
      pthread_mutex_init(&mutex_pista[i][j], NULL);
    }

  /* Condições iniciais dos ciclistas */
  for (int i = 0; i < n; i++) {
    metro(i) = d - (i / (FAIXAS / 2)) - 1;

    faixa(i) = i % (FAIXAS / 2) + (i / (FAIXAS / 2) % 2) * (FAIXAS / 2);
    pista[metro(i)][faixa(i)] = i + 1;

    velocidade(i) = 30;
    volta_atual(i) = -1;
    quando_quebrou(i) = -1;
    chegou(i) = -1;
  }

  /* Imprimir a condição inicial caso debugOn */
  if (debugOn) debugar();

  /* Iniciando os ciclistas */
  int *temp;
  for (int i = 0; i < n; i++) {
    temp = malloc(sizeof(int));
    *temp = i;
    if (debugOn) fprintf(stderr, "Criando %d\n", i);
    pthread_create(&threads[i], NULL, ciclista, (void *) temp);
  }

  /* Esperando todos terminarem a corrida */
  for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);

  /* Liberando os mutexes, barreiras e nossas listas ligadas */
  pthread_mutex_destroy(&mutex_barreira);
  pthread_mutex_destroy(&mutex_eliminado);

  for (int i = 0; i < d; ++i)
    for (int j = 0; j < FAIXAS; ++j) pthread_mutex_destroy(&mutex_pista[i][j]);

  pthread_barrier_destroy(&barreira);

  for (int i = 0; i < 2 * n; i++)
    if (ranking[i]) freeList(ranking[i]);

  /* Fim da corrida. Obrigado aos ciclistas participantes =) */
  return 0;
}
