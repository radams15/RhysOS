#include "malloc.h"

#include "sysinfo.h"

static int heap = 300000;

void memmgr_init() {
	/*int mem_low = lowmem();
	int mem_high = highmem();*/
	
	/*if(mem_high != 0) {
		heap = mem_high * 1000;
	} else {
		heap = mem_low * 1000;
	}*/
	
	//heap = HEAP_ADDRESS;
}

void* malloc(unsigned int size) {
	heap -= size;

	return heap;
}

/*void* malloc(unsigned int size) {
	int out = heap;
	
	heap += size;

	return (int*) out;
}*/

void free(void* ptr) {
	// TODO implement memory management.
}
