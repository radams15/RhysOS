#ifndef COMPAT_C
#define COMPAT_C

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

int fgets_int(char* buffer, int len, int fh) {
    char c;
    int n = 0;

    do {
        c = fgetch(fh);
        buffer[n] = c;
        putc_ovr(c);
        n++;

        lseek(fh, 1, SEEK_CUR);
    } while (c != '\n' && n<len-1);

    buffer[n] = 0;  // null-terminate

    if(lseek(fh, 1, SEEK_CUR) == 0 || (strlen(buffer) > 0 && buffer[0] == '\n')) {
        return 0;
    }

    lseek(fh, -1, SEEK_CUR);

    return n;
}

#define fgets fgets_int
#define putc putc_ovr
#define seek lseek

#endif
