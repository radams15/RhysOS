#ifndef STDLIB_H
#define STDLIB_H

#include "stddef.h"

typedef struct TimeDelta {
    int tick;
    int sec;
    int min;
    int hr;
} TimeDelta_t;


void memset(int* buf, int val, int n);
void memcpy(uint8* dst, uint8* src, int n);

#endif
