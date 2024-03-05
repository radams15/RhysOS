//
// Created by rhys on 05/03/24.
//

#include <malloc.h>
#include "malloc.h"

unsigned char* heap_begin;
unsigned char* heap_end;
unsigned char* heap;

const static unsigned char magic = 0xBE;

unsigned int heap_size = 640*1024; // 640k memory

struct BlkHeader {
    unsigned char magic;
    unsigned int length;
    unsigned char* next;
    unsigned char free;
};

void error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}

int defn_header(unsigned char* ptr, unsigned len, unsigned char* next, unsigned char free) {
    struct BlkHeader* header = (struct BlkHeader*) ptr;

    header->magic = magic;
    header->length = len;
    header->next = next;
    header->free = free;

    return 0;
}

struct BlkHeader* parse_block(unsigned char* ptr) {
    struct BlkHeader* header = (struct BlkHeader*) ptr;

    if(ptr == NULL) {
        error("Invalid block parse (NULL)");
        return NULL;
    }

    if(header->magic != magic) {
        error("Invalid block magic!");
        return NULL;
    }

    return header;
}

struct BlkHeader* split(struct BlkHeader* block, unsigned int size) {
    struct BlkHeader* out = block;

    unsigned int old_size = block->length;
    unsigned int new_size = old_size - (size + sizeof(struct BlkHeader));

    block += size + sizeof(struct BlkHeader); // increment block pointer

    defn_header((unsigned char*) block, new_size, block->next, 1);
    defn_header((unsigned char*) out, size, (unsigned char*) block, 1);

    return out;
}

unsigned char* kmalloc(unsigned int size) {
    if(size == 0) {
        return NULL;
    }

    struct BlkHeader* header = parse_block(heap_begin);

    int i=0;
    while(header != NULL) {
        i++;
        if(header->length >= size && header->free) {
            goto found_block;
        }

        header = parse_block(header->next);
    }

    return NULL;

found_block:

    if(header->length > size) {
        header = split(header, size);
    }

    printf("%d blocks\n", i);

    defn_header((unsigned char*) header, header->length, header->next, 0);

    unsigned char* out = ((unsigned char*) header) + sizeof(struct BlkHeader);

    return out;
}

// int defn_header(unsigned char* ptr, unsigned len, unsigned char* next, unsigned char free)
void kfree(unsigned char *ptr) {
    ptr -= sizeof(struct BlkHeader);

    struct BlkHeader* header = parse_block(ptr);

    if(header == NULL) {
        error("Invalid kernel free");
        return;
    }

    struct BlkHeader* next = parse_block(header->next);
    unsigned int length = header->length;
    if(next->free) {
        length += next->length + sizeof(struct BlkHeader);
    }

    defn_header((unsigned char*) header, length, (unsigned char*) next, 1);
}

int memmgr_init() {
    heap = malloc(heap_size * sizeof(char));

    heap_begin = heap;
    heap_end = heap_begin + heap_size;

    defn_header(heap_begin, heap_size-sizeof(struct BlkHeader), NULL, 1);

    return 0;
}