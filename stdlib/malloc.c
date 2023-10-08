#include "malloc.h"
#include "stdio.h"

extern unsigned int heap_begin;
extern unsigned int heap_end;

static unsigned int heap;

#define HEAP_MAGIC 0xCAFE
#define BLOCK_SIZE (0x100-sizeof(BlkHeader_t))    // size-blocks that can be allocated

#define CEIL_DIV(x, y) ((x/y) + (x % y != 0))

// https://wiki.osdev.org/Page_Frame_Allocation

typedef struct BlkHeader {
    unsigned int magic;   // HEAP_MAGIC
    unsigned int length;  // Length of the block
    struct BlkHeader* next;    // Pointer to next block
    unsigned int free;  // Is it being used?
} BlkHeader_t;

void memmgr_init() {
    heap = &heap_begin;
    
    BlkHeader_t* header = (BlkHeader_t*) &heap_begin;
        
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
    
    return out;
}

int align(int n) {
    return (n + sizeof(int) - 1) & ~(sizeof(int) - 1);
}

void* malloc(unsigned int size) {
    if(size == 0)
        return 0;

    size = align(size);
    
    BlkHeader_t* header = (BlkHeader_t*) &heap_begin;
    while(1) {
        if(header == NULL) {
            fprintf(stderr, "Memory allocation error (out of blocks)!\n");
            return 0;
        }
        if(header->magic != HEAP_MAGIC) {
            fprintf(stderr, "Memory allocation error (invalid magic = %x)!\n", header->magic);
            return 0;
        }
        
        if(header->length >= size && header->free) {
            break;
        }
        
        header = header->next;
    }
    
    if(header->length > size) {
        header = split(header, size);
    }
    
    header->magic = HEAP_MAGIC;
    header->free = 0;

    return (int*)header + sizeof(BlkHeader_t);
}

void free(void* ptr) {
    BlkHeader_t* header = ptr - sizeof(BlkHeader_t);
    
    if(header->magic != HEAP_MAGIC) {
        fprintf(stderr, "Invalid free of pointer %x (magic = %x)\n", ptr, header->magic);
        return;
    }
    
    if(header->next->free) {
        header->next = header->next->next;
        header->length += header->next->length + sizeof(BlkHeader_t);
    }
    
    header->free = 1;
    
    printf("Valid free\n");
    // TODO implement memory management.
}

