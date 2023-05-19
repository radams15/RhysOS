#include "malloc.h"

#include "sysinfo.h"

static int heap_top;
static int heap;

#define HEAP_MAGIC 0xCAFE

typedef struct BlkHeader {
	unsigned int magic; // HEAP_MAGIC
	unsigned int length; // Length of the block
} BlkHeader_t;

void memmgr_init() {
	/*int mem_low = lowmem();
	int mem_high = highmem();*/
	
	/*if(mem_high != 0) {
		heap = mem_high * 1000;
	} else {
		heap = mem_low * 1000;
	}*/
	
	heap_top = HEAP_ADDRESS;
	heap = heap_top;
}

/*void* malloc(unsigned int size) {
	heap -= size;

	return heap;
}*/

void* malloc(unsigned int size) {
	BlkHeader_t* out = heap;
	
	heap += size + sizeof(BlkHeader_t) + 8;
	
	clear(out, heap-(int)out);
	
	out->magic = HEAP_MAGIC;
	out->length = size;

	return (int*) out + sizeof(BlkHeader_t);
}

void free(void* ptr) {
	// TODO implement memory management.
}
