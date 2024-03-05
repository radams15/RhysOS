//
// Created by rhys on 05/03/24.
//

#ifndef MALLOC_MALLOC_H
#define MALLOC_MALLOC_H

int memmgr_init();
unsigned char* kmalloc(unsigned int size);
void kfree(unsigned char* ptr);

#endif //MALLOC_MALLOC_H