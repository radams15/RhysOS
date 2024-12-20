#ifndef MALLOC_H
#define MALLOC_H

struct BlkHeader {
    unsigned char magic;
    unsigned int length;
    unsigned char* next;
    unsigned char free;
};

int memmgr_init();
void* malloc(unsigned int size);
void* realloc(void* ptr, unsigned int size);
void free(void* ptr);

struct BlkHeader* mem_get_header(void* ptr);

int a20_available();
int a20_enable();

unsigned int text_begin_addr;
unsigned int text_end_addr;
unsigned int data_begin_addr;
unsigned int data_end_addr;
unsigned int heap_begin_addr;
unsigned int heap_end_addr;

#endif
