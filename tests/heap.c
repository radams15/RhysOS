#include <stdio.h>

#define HEAP_ADDRESS 0x20000

static int heap_c[512];
static int* heap = (int*) &heap_c;

void* alloc(unsigned int size) {
	void* out = heap;

	heap += size;

	return out;
}

void free(void* ptr) {
	// TODO implement memory management.
}

int main() {
	int* addr;
	for(int i=0 ; i<10 ; i++) {
		addr = alloc(10);

		*addr = i;

		printf("Base+%x = %p (%d)\n", 10, addr, *addr);
	}
}
