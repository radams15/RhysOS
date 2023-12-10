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

File_t* fopen(const char* fname, const char* mode) {
    FileMode_t mode_val = O_RDONLY;

    int fh = open(fname, mode_val);

    for(char* c=mode ; *c != NULL ; c++) {
        if(*c == 'w')
            mode_val |= O_CREAT;
            mode_val &= ~O_RDONLY;
        if(*c == 'a')
            mode_val |= O_APPEND;
    }

    if(fh == -1) {
        return NULL;
    }

    File_t* out = malloc(sizeof(File_t));
    out->fh = fh;

    return out;
}

int fwrite(File_t* file, unsigned char* buffer, unsigned int size) {
    return write(file->fh, buffer, size);
}

int fclose(File_t* file) {
    int out = close(file->fh);

    free(file);

    return out;
}

int fseek(File_t* file, int pos) {
    return seek(file->fh, pos);
}
