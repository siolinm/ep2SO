#include "util.h"

#include <stdio.h>

#define min(a, b) (a < b ? a : b)

#define COM_COR 0

void debugar() {
  int soma = 0;

  t_sec_cur += t_cur / 1000;
  t_cur %= 1000;

  fprintf(stderr, "\ntempo: %d s %d ms\n", t_sec_cur, t_cur);
  for (int k = 0; k < d; k += TRECHO) {
    fprintf(stderr, "    ");

    for (int i = k; i < min(k + TRECHO, d); i++) fprintf(stderr, " %5d", i + 1);
    fprintf(stderr, "\n    ");

    for (int i = k; i < min(k + TRECHO, d); i++) fprintf(stderr, "------");
    fprintf(stderr, "\n");

    for (int j = 0; j < FAIXAS; j++) {
      fprintf(stderr, "%2d |", j + 1);
      for (int i = k; i < min(k + TRECHO, d); i++) {
        if (pista(i, j) == 0)
          fprintf(stderr, " %5d", pista(i, j));
        else if (COM_COR) // 32 -> verde
          fprintf(stderr, " \033[1;%dm%5d\033[0m", 32, pista(i, j));
        else
          fprintf(stderr, " %5d", pista(i, j));
        soma += pista(i, j);
      }
      fprintf(stderr, "\n");
    }
  }

  /* if (n == n_cur && soma != (n + 1) * n / 2) fprintf(stderr, "ERRO\n"); */
}
