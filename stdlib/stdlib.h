#ifndef STDLIB_H
#define STDLIB_H

#include "stddef.h"

typedef struct TimeDelta {
    int tick;
    int sec;
    int min;
    int hr;
} TimeDelta_t;

typedef struct File {
    int fh;
} File_t;

void memset(int* buf, int val, int n);
void memcpy(uint8* dst, uint8* src, int n);

File_t* fopen(const char* fname, const char* mode);
int fwrite(File_t* file, unsigned char* buffer, unsigned int size);
int fclose(File_t* file);
int fseek(File_t* file, int pos);


#endif
