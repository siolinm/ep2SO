#include "util.h"

#include <stdio.h>

#define min(a, b) (a < b ? a : b)

void setPosicao(int PID, int i, int j) {
    metro(PID) = i;
    faixa(PID) = j;
}

void setvPosicao(int PID, int pos[2]) {
    metro(PID) = pos[0];
    faixa(PID) = pos[1];
}

void debugar() {
    char c;
    printf("tempo: %d ms\n", t_cur);
    for (int k = 0; k < d; k += TRECHO) {
        printf("    ");

        for (int i = k; i < min(k + TRECHO, d); i++) printf(" %5d", i + 1);
        printf("\n    ");

        for (int i = k; i < min(k + TRECHO, d); i++) printf("------");
        printf("\n");

        for (int j = 0; j < FAIXAS; j++) {
            printf("%2d |", j + 1);
            for (int i = k; i < min(k + TRECHO, d); i++)
                if (pista(i, j) == 0)
                    printf(" %5d", pista(i, j));
                else
                    printf(" \033[1;%dm%5d\033[0m", 32, pista(i, j));

            printf("\n");
        }
    }
    scanf(" %c", &c);
}
