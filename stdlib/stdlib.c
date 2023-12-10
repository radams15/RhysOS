#include "stdlib.h"
#include "syscall.h"
#include "stdio.h"
#include "malloc.h"

void memset(int* buf, int val, int n) {
    for (; n != 0; n--)
        buf[n] = val;
}

void memcpy(uint8* dst, uint8* src, int n) {
    for (; n != 0; n--) {
        dst[n] = src[n];
    }
}

void abort() {
    fprintf(stderr, "Program abort!\n");
}

