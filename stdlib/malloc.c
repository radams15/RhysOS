#include "malloc.h"
#include "stdio.h"

extern unsigned int heap_begin;
extern unsigned int heap_end;

static unsigned int heap;

#define HEAP_MAGIC 0xCAFE
#define BLOCK_SIZE \
    (0x100 - sizeof(BlkHeader_t))  // size-blocks that can be allocated

// https://wiki.osdev.org/Page_Frame_Allocation

typedef struct BlkHeader {
    unsigned int magic;      // HEAP_MAGIC
    unsigned int length;     // Length of the block
    struct BlkHeader* next;  // Pointer to next block
    unsigned int free;       // Is it being used?
} BlkHeader_t;

void memmgr_init() {
    heap = &heap_begin;

    BlkHeader_t* header = (BlkHeader_t*)&heap_begin;

    printf("Free memory size: %x\n", &heap_end-(unsigned int) header);

    header->magic = HEAP_MAGIC;
    header->length = &heap_end - &heap_begin;
    header->free = 1;

    header->next = NULL;
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

void* malloc(uint16 size) {
    if (size == 0)
        return 0;

    size += sizeof(BlkHeader_t);

    BlkHeader_t* header = (BlkHeader_t*)&heap_begin;
    while (1) {
        if (header == NULL) {
            fprintf(stderr, "Memory allocation error (out of blocks)!\n");
            return 0;
        }
        if (header->magic != HEAP_MAGIC) {
            fprintf(stderr, "Memory allocation error (invalid magic = %x)!\n",
                    header->magic);
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

    unsigned int* out = ((unsigned int*)header) + sizeof(BlkHeader_t);

    printf("Allocated %d bytes (%x)\n", header->length, out);

    return (void*)out;
}

void* calloc(uint16 n, uint16 size) {
    void* out = malloc(n * size);

    memset(out, 0, n);

    return out;
}

void* realloc(void* ptr, uint16 size) {
    if(ptr == NULL)
        return malloc(size);
    else if(size == 0) {
        free(ptr);
        return NULL;
    }

    BlkHeader_t* header = (unsigned int*)ptr - sizeof(BlkHeader_t);

    int extra = size - header->length;

    BlkHeader_t* new_header;
    if (header->next->length >= extra) {
        new_header = split(header->next, size);
        header->length += new_header->length;
    } else {
        new_header = malloc(size);
        uint16* dst =
            (void*)(((unsigned int*)new_header) + sizeof(BlkHeader_t));
        memcpy(dst, ptr, header->length);
        free(ptr);
    }

    return (void*)new_header;
}

void free(void* ptr) {
    BlkHeader_t* header = (unsigned int*)ptr - sizeof(BlkHeader_t);

    if (header->magic != HEAP_MAGIC) {
        fprintf(stderr, "Invalid free of pointer %x (magic = %x)\n", header,
                header->magic);
        return;
    }

    printf("Free'd %d bytes (%x)\n", header->length, ptr);

    if (header->next != NULL && header->next->free) {
        header->next = header->next->next;
        header->magic = HEAP_MAGIC;
        header->length += header->next->length + sizeof(BlkHeader_t);
    }

    header->free = 1;
}
