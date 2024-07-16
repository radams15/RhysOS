#ifndef COMPAT_C
#define COMPAT_C

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define stdin 0
#define stdout 1
#define stderr 2

void putc_ovr(char c) {
    printf("%c", c);
}

char fgetch(int fh) {
    char c;

    read(fh, &c, 1);

    lseek(fh, -1, SEEK_CUR);

    return c;
}

#define putc putc_ovr
#define seek lseek

#endif
