#include "util.h"

void setPosicao(int PID, int i, int j) {
	metro(PID) = i;
	faixa(PID) = j;
}

void setvPosicao(int PID, int pos[2]) {
	metro(PID) = pos[0];
	faixa(PID) = pos[1];
}
