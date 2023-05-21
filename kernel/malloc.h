#ifndef MALLOC_H
#define MALLOC_H

void memmgr_init();
void* malloc(unsigned int size);
void free(void* ptr);

int a20_available();
int a20_enable();

#endif
