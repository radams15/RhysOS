#ifndef MALLOC_H
#define MALLOC_H

int memmgr_init();
void* malloc(unsigned int size);
void* realloc(void* ptr, unsigned int size);
void free(void* ptr);

int a20_available();
int a20_enable();

#endif
