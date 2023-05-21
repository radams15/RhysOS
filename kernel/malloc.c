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
	
	if(heap+size > lowmem()*1000) {
		print_string("Cannot allocate more memory!\n");
		return 0;
	}
	
	heap += size + sizeof(BlkHeader_t) + 8;
	
	clear(out, heap-(int)out);
	
	out->magic = HEAP_MAGIC;
	out->length = size;

	return (int*) out + sizeof(BlkHeader_t);
}

void free(void* ptr) {
	// TODO implement memory management.
}

int a20_available() {
	return interrupt(0x15, 0x2403, 0, 0, 0);
}

int a20_enable() {
	int enabled;
	
	enabled = interrupt(0x15, 0x2402, 0, 0, 0); // is the gate enabled?
	
	if(enabled) // Already enabled!
		return 0;
	
	interrupt(0x15, 0x2401, 0, 0, 0); // Enable a20

	enabled = interrupt(0x15, 0x2402, 0, 0, 0); // is the gate enabled?
	
	return enabled != 0;
}
