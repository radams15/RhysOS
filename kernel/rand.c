#include "rand.h"

#define A 75
#define C 74
#define M 65535

static unsigned int next = 1;

int rand() {
    next = (A*next+C) % M;
    return next;
}

void srand(unsigned int seed) {
    next = seed;
}
