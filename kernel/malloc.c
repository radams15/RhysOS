#include "malloc.h"

static int heap = HEAP_ADDRESS;

void* malloc(unsigned int size) {
	int out = heap;

	heap += size;

	return (int*) out;
}

void free(void* ptr) {
	// TODO implement memory management.
}
