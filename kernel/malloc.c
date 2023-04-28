#include "malloc.h"

static int* heap = (int*) HEAP_ADDRESS;

void* malloc(unsigned int size) {
	void* out = heap;

	heap += size;

	return out;
}

void free(void* ptr) {
	// TODO implement memory management.
}
