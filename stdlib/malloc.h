#ifndef MALLOC_H
#define MALLOC_H

#include "stddef.h"

void* malloc(uint16 size);
void* calloc(uint16 n, uint16 size);

void* realloc(void* ptr, uint16 size);

void free(void* ptr);

#endif
