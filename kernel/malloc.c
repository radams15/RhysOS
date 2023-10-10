#include "malloc.h"

#include "sysinfo.h"

extern int heap_begin;
extern int heap_end;
static int heap;

#define HEAP_MAGIC 0xCAFE

typedef struct BlkHeader {
    unsigned int magic;   // HEAP_MAGIC
    unsigned int length;  // Length of the block
} BlkHeader_t;

int memmgr_init() {
    heap = &heap_begin;
    return 0;
}

void* malloc(unsigned int size) {
    BlkHeader_t* out = heap;

    printi(heap, 16);printi(size, 16); print_string(";");

    if (heap + size > &heap_end) {
        print_string("Cannot allocate more memory!\n");
        return 0;
    }

    heap += size + sizeof(BlkHeader_t) + 8;

    out->magic  = HEAP_MAGIC;
    out->length = size;

    return (int*)out + sizeof(BlkHeader_t);
}

void free(void* ptr) {
    // TODO implement memory management.
}

int a20_available() {
    return interrupt(0x15, 0x2403, 0, 0, 0);
}

int a20_enable() {
    int enabled;

    enabled = interrupt(0x15, 0x2402, 0, 0, 0);  // is the gate enabled?

    if (enabled)  // Already enabled!
        return 0;

    interrupt(0x15, 0x2401, 0, 0, 0);  // Enable a20

    enabled = interrupt(0x15, 0x2402, 0, 0, 0);  // is the gate enabled?

    return enabled != 0;
}
