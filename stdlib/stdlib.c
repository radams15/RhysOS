#include "stdlib.h"
#include "malloc.h"
#include "stdio.h"
#include "syscall.h"

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
