#include "malloc.h"
#include "type.h"

extern unsigned int heap_begin;
extern unsigned int heap_end;
static unsigned int heap;

#define HEAP_MAGIC 0xCAFE

typedef struct BlkHeader {
    unsigned int magic;      // HEAP_MAGIC
    unsigned int length;     // Length of the block
    struct BlkHeader* next;  // Pointer to next block
    unsigned int free;       // Is it being used?
} BlkHeader_t;

int memmgr_init() {
    heap = &heap_begin;

    BlkHeader_t* header = (BlkHeader_t*)&heap_begin;

    header->magic = HEAP_MAGIC;
    header->length = &heap_end - &heap_begin;
    header->free = 1;

    header->next = NULL;

    return 0;
}

BlkHeader_t* split(BlkHeader_t* block, unsigned int size) {
    BlkHeader_t* out = block;

    int old_size = block->length;

    block += size + sizeof(BlkHeader_t);
    block->magic = HEAP_MAGIC;
    block->length = old_size - (size + sizeof(BlkHeader_t));
    block->free = 1;

    out->length = size;
    out->next = block;
    out->magic = HEAP_MAGIC;
    out->free = 1;

    return out;
}

void* malloc(unsigned int size) {
    if (size == 0)
        return 0;

    size += sizeof(BlkHeader_t);

    BlkHeader_t* header = (BlkHeader_t*)&heap_begin;
    while (1) {
        if (header == NULL) {
            print_string("Kernel memory allocation error (out of blocks)!\n");
            return 0;
        }

        if (header->magic != HEAP_MAGIC) {
            print_string(
                "Kernel memory allocation error (invalid heap magic)!\n");
            return 0;
        }

        if (header->length >= size && header->free) {
            break;
        }

        header = header->next;
    }

    if (header->length > size) {
        header = split(header, size);
    }

    header->magic = HEAP_MAGIC;
    header->free = 0;

    unsigned int out = ((unsigned int)header) + sizeof(BlkHeader_t);

    // print_string("Malloc: ");
    // printi(out, 16);
    // print_string("\n");

    return (void*)out;
}

void free(void* ptr) {
    // print_string("Free: ");
    // printi(ptr, 16);
    // print_string("\n");

    BlkHeader_t* header = (unsigned int)ptr - sizeof(BlkHeader_t);

    if (header->magic != HEAP_MAGIC) {
        print_string("Invalid kernel free!\n");
        return;
    }

    if (header->next->free) {
        header->next = header->next->next;
        header->length += header->next->length + sizeof(BlkHeader_t);
    }

    header->free = 1;
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
