#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "linkedList.h"
#include "util.h"

#define DEBUG_COND 0

Bool chegouChegada(int PID) {
  double rand_value;
  int aux;
  Bool quebrou_ou_eliminado = False;
  Bool atingiu_max = False;

  /* chegou na chegada, entao incrementamos a volta do ciclista */
  volta_atual(PID) += 1;
  t_eliminado(PID) = t_sec_cur * 1000 + t_cur;

  /* caso de exceção para a primeira volta */
  if (volta_atual(PID) == 0) {
    pthread_mutex_lock(&mutex_eliminado);
    if (ranking[0] == NULL) // primeiro a chegar
      ranking[0] = initList();
    push(ranking[0], PID);

    if (ranking[0]->size == n) ult++;

    pthread_mutex_unlock(&mutex_eliminado);
    return False;
  }

  pthread_mutex_lock(&mutex_eliminado);

  /* ver se deveria já ter sido eliminado */
  if (eliminado(PID)) {
    n_cur--;

    if (velocidade(PID) == 90) // PID ñ deveria estar a 90 (foi eliminado antes)
      alguem_a_90 = 2;

    pthread_mutex_unlock(&mutex_eliminado);
    return True;
  }

  /* ver se o ciclista já terminou a corrida e deve sair da pista */
  if (volta_atual(PID) >= max_voltas) {
    n_terminaram++;

    fprintf(stderr, "Ciclista %d parou de correr.\n", PID + 1);
    atingiu_max = True;
  }

  /* ver se quebrou? */
  if (!atingiu_max && n_cur > 5 && volta_atual(PID) != 1 &&
      volta_atual(PID) % 6 == 1) {
    rand_value = (((double) rand()) / RAND_MAX);
    if (rand_value < 0.05) { /* quebrou */
      quebrou_ou_eliminado = True;
      eliminado(PID) = volta_atual(PID) - 1;
      if (quebraram[volta_atual(PID)] == NULL)
        quebraram[volta_atual(PID)] = initList();
      push(quebraram[volta_atual(PID)], PID);

      t_quebrou(PID) = t_sec_cur * 1000 + t_cur;

      fprintf(stderr, "Ciclista %d quebrou após completar a volta %d.\n",
              PID + 1, volta_atual(PID) - 1);
    }
  }

  /* se não queboru ou foi eliminado, então classificou para a próxima volta */
  if (!quebrou_ou_eliminado) {
    if (ranking[volta_atual(PID)] == NULL) // primeiro a chegar
      ranking[volta_atual(PID)] = initList();

    fprintf(stderr, "PID %d classificou na volta %d.\n", PID + 1,
            volta_atual(PID));
    push(ranking[volta_atual(PID)], PID);
  }

  /* caso ele seja o último de uma volta, devemos processa-la  */
  aux = (ult == 1 ? 0 : ult % 2);
  fprintf(stderr, "%d + %d = %d - %d\n", getSize(ranking[ult]),
          getSize(quebraram[ult]), getSize(ranking[ult - 1]), aux);
  while (ranking[ult] && ult && ranking[ult]->size != 1 &&
         ranking[ult]->size + getSize(quebraram[ult]) ==
             ranking[ult - 1]->size - aux) {
    if (ult % 2 == 0) {
      int i = getLast(ranking[ult]);
      eliminado(i) = ult; // aviso para quando ele passar pela chegada
      max_voltas = ult + 2 * (ranking[ult]->size - 2);

      fprintf(stderr, "Ciclista %d eliminado na volta %d.\n", i + 1, ult);
    }

    fprintf(stderr, "Volta completada nº %d:\n", ult);
    print(ranking[ult]);

    ult++;

    if (ranking[ult]) update(ranking[ult]);
    if (quebraram[ult]) update(quebraram[ult]);
    aux = (ult == 1 ? 0 : ult % 2);
  }

  /* se na hora que eu cruzei, vi que eu era o último, então estou eliminado */
  quebrou_ou_eliminado = (quebrou_ou_eliminado || eliminado(PID));
  if (!atingiu_max && quebrou_ou_eliminado) {
    n_cur--;

    if (velocidade(PID) == 90) // PID ñ deveria estar a 90 (foi eliminado antes)
      alguem_a_90 = 2;
  }

  pthread_mutex_unlock(&mutex_eliminado);

  /* mudar a minha velocidade */
  if (!atingiu_max && !quebrou_ou_eliminado) {
    rand_value = (((double) rand()) / RAND_MAX);
    if (velocidade(PID) == 30 && rand_value < 0.8)
      velocidade(PID) = 60;
    else if (velocidade(PID) == 60 && rand_value < 0.4)
      velocidade(PID) = 30;

    pthread_mutex_lock(&mutex_eliminado);
    if (alguem_a_90 > 0 && max_voltas - volta_atual(PID) <= 2) {
      if (alguem_a_90 == 1) {
        rand_value = (((double) rand()) / RAND_MAX);
        if (rand_value < 0.5) { // eu fico a 90
          velocidade(PID) = 90;
          alguem_a_90 = -1;
        } else
          alguem_a_90 = 2; // o segundo fica

      } else {
        velocidade(PID) = 90;
        alguem_a_90 = -1;
      }
    }
    pthread_mutex_unlock(&mutex_eliminado);
  }

  return (quebrou_ou_eliminado || atingiu_max);
}

/* função chamada quando o ciclista anda um metro para frente */
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

  /* A thread roda até ser eliminada  */
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
        usleep(2000);
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
        ciclistaEliminado = True;
        pista(metro(PID), faixa(PID)) = 0;
        pthread_mutex_unlock(&mutex_eliminado);
      }

      pthread_mutex_unlock(&mutex_pista[i][j]);
      max_round(PID) = 180 / velocidade(PID);
    }

    /* Antes de terminar a ação, incrementamos o round */
    round(PID) = (round(PID) + 1) % max_round(PID);

    /* Terminou a ação  */
    terminou_acao(PID) = True;

    pthread_barrier_wait(&barreira);

    /* Ações de sincronização */
    if (fuiEliminado) break;
    pthread_mutex_lock(&mutex_barreira);

    terminou_acao(PID) = False;
    if (n_cur == 1) { fuiEliminado = True; }

    if (primeiro_a_chegar) {
      primeiro_a_chegar = False;

      if (ciclistaEliminado) {
        pthread_barrier_destroy(&barreira);
        pthread_barrier_init(&barreira, NULL, n_cur - n_terminaram);
        ciclistaEliminado = False;
      }

      if (debug_on && (t_cur % 60 == 0 || alguem_a_90 == -1)) debugar();

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
  srand(time(NULL));

  /* Pegando parâmetros da entrada */
  d = atoi(argv[1]);
  n = atoi(argv[2]);

  /* Definindo variáveis globais */
  n_cur = n;
  t_cur = 0;
  t_sec_cur = 0;
  ult = 0;
  n_terminaram = 0;
  max_voltas = 2 * (n - 1);
  alguem_a_90 = ((((double) rand()) / RAND_MAX) < 0.1);
  debug_on = DEBUG_COND;

  for (int i = 0; i < 2 * n; i++) quebraram[i] = ranking[i] = NULL;

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
    t_quebrou(i) = -1;
    t_eliminado(i) = -1;
    eliminado(i) = 0;
  }

  /* Imprimir a condição inicial caso debug_on */
  if (debug_on) debugar();

  /* Iniciando os ciclistas */
  int *temp;
  for (int i = 0; i < n; i++) {
    temp = malloc(sizeof(int));
    *temp = i;
    pthread_create(&threads[i], NULL, ciclista, (void *) temp);
  }

  char myCmd[1000];
  sprintf(myCmd,
          "{ pmap $(pgrep ep2) | grep total ; } >> relatorio%d-%d-%d.txt", d,
          n, atoi(argv[3]));
  system(myCmd);

  /* Esperando todos terminarem a corrida */
  for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);

  /* Estatísticas do fim da corrida  */
  int j = n - 1;
  for (int i = 0; i < n; i++) {
    if (t_quebrou(i) != -1)
      rank_final[j--] = i;
    else
      rank_final[eliminado(i) / 2] = i;
    fprintf(stderr, "eliminado(%d) = %d\n", i + 1, eliminado(i));
  }

  fprintf(stderr, "\n\nRank final:\n\t%dº: %d no instante %d ganhou.\n", 1,
          rank_final[0] + 1, t_eliminado(rank_final[0]));
  for (int i = j; i >= 1; i--) {
    fprintf(stderr, "\t%dº: %d no instante %d após completar a volta %d.\n",
            j - i + 2, rank_final[i] + 1, t_eliminado(rank_final[i]),
            eliminado(rank_final[i]));
  }
  fprintf(stderr, "Quebrados:\n");
  for (++j; j < n; j++) {
    fprintf(stderr, "\t%d quebrou no instante %d após completar a volta %d.\n",
            rank_final[j] + 1, t_quebrou(rank_final[j]),
            eliminado(rank_final[j]));
  }

  /* Liberando os mutexes, barreiras e nossas listas ligadas */
  pthread_mutex_destroy(&mutex_barreira);
  pthread_mutex_destroy(&mutex_eliminado);

  for (int i = 0; i < d; ++i)
    for (int j = 0; j < FAIXAS; ++j) pthread_mutex_destroy(&mutex_pista[i][j]);

  pthread_barrier_destroy(&barreira);

  for (int i = 0; i < 2 * n; i++)
    if (ranking[i]) freeList(ranking[i]);

  fprintf(stderr,
          "\n\nFim da corrida. Obrigado aos ciclistas participantes =)\n");
  /* Fim da corrida. Obrigado aos ciclistas participantes =) */
  return 0;
}

/* 3, 5, 2 */

