#include "malloc.h"
#include "stdio.h"

extern unsigned int heap_begin;
extern unsigned int heap_end;
static unsigned int heap;

#define HEAP_MAGIC 0xCAFE

typedef struct BlkHeader {
    unsigned int magic;   // HEAP_MAGIC
    unsigned int length;  // Length of the block
} BlkHeader_t;

void memmgr_init() {
    heap = &heap_begin;
}

void* malloc(unsigned int size) {
    BlkHeader_t* out = heap;

    if (heap + size > &heap_end) {
        printf("Cannot allocate more memory (%x > %x)!\n", heap+size, &heap_end);
        return 0;
    }

    heap += size + sizeof(BlkHeader_t) + 8;

    memset(out, heap - (int)out, 0);

    out->magic = HEAP_MAGIC;
    out->length = size;

    return (int*)out + sizeof(BlkHeader_t);
}

void free(void* ptr) {
    // TODO implement memory management.
}

