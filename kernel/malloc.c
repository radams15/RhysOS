#include "malloc.h"
#include "interrupt.h"
#include "tty.h"
#include "type.h"
#include "util.h"




unsigned char* heap_begin;
unsigned char* heap_end;
unsigned char* heap;
unsigned int heap_size;

const static unsigned char magic = 0xBE;

void error(const char* msg) {
    print_string((char*) msg);
    print_char('\n');
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
        // error("Invalid block parse (NULL)"); 
        return NULL;
    }

    if(header->magic != magic) {
        // error("Invalid block magic!"); 
        return NULL;
    }

    return header;
}

struct BlkHeader* mem_get_header(void* ptr) {
    return parse_block(ptr - sizeof(struct BlkHeader));
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

void* malloc(unsigned int size) {
    if(size == 0) {
        return NULL;
    }

    struct BlkHeader* header = parse_block(heap_begin);

    if(header == NULL) {
        printf("Malloc error: failed to find initial header\n");
        return NULL;
    }

    int i=0;
    while(header != NULL) {
        i++;
        /* printf("Block: %x\n", header); */
        if(header->length >= size && header->free) {
            goto found_block;
        }

        header = parse_block(header->next);
    }

    printf("kmalloc: failed to allocate memory\n");
    return NULL;

found_block:
    if(header->length > size) {
        header = split(header, size);
    }

    defn_header((unsigned char*) header, header->length, header->next, 0);

    unsigned char* out = ((unsigned char*) header) + sizeof(struct BlkHeader);

    /* printf("kmalloc (%d): %x\n", size, out); */
    return out;
}

void condense_memory() {
    struct BlkHeader* header = parse_block(heap_begin);
    struct BlkHeader* next;
    unsigned int length;

    while(header != NULL) {
        next = parse_block(header->next);

        if(next == NULL) {
            break;
        }

        if(header->free && next->free) {
            length = header->length + next->length + (2 * sizeof(struct BlkHeader));
        
            defn_header((unsigned char*) header, length, (unsigned char*) next->next, 1);
        }

        header = next;
    }
    printf("\n");
}

void free(void* ptr) {
    struct BlkHeader* header = mem_get_header(ptr);

    if(header == NULL) {
        printf("Invalid kernel free (%x)\n", ptr);
        return;
    }

    /* printf("kfree: %x\n", ptr); */

    header->free = TRUE;

    condense_memory();
}

int memmgr_init() {
    heap = (unsigned char*) &heap_begin_addr;
    heap_size = (unsigned char*) &heap_end_addr - (unsigned char*) &heap_begin_addr;

    heap_begin = heap;
    heap_end = heap_begin + heap_size;

    defn_header(heap_begin, heap_size-sizeof(struct BlkHeader), NULL, 1);

    return 0;
}











void* realloc(void* ptr, unsigned int size) {
    /*if (ptr == NULL)
        return malloc(size);
    else if (size == 0) {
        free(ptr);
        return NULL;
    }

    BlkHeader_t* header =
        (BlkHeader_t*)((unsigned int*)ptr - sizeof(BlkHeader_t));

    int extra = size - header->length;

    BlkHeader_t* new_header;
    if (header->next->length >= extra) {
        new_header = split(header->next, size);
        header->length += new_header->length;
    } else {
        new_header = malloc(size);
        unsigned int dst =
            (unsigned int)(((unsigned int*)new_header) + sizeof(BlkHeader_t));
        memcpy((char*)dst, ptr, header->length);
        free(ptr);
    }

    return (void*)new_header;*/
    error("Realloc unimplemented!");
    return NULL;
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
