#include <stdio.h>

int lowmem() {
	int fh;
	int mem;

	char buf[2];
	fh = open("/dev/lowmem");
	read(fh, &buf, sizeof(&buf));
	close(fh);

	mem = chars2int(buf);

	return mem;
}

int highmem() {
	int fh;
	int mem;

	char buf[2];
	fh = open("/dev/highmem");
	read(fh, &buf, sizeof(&buf));
	close(fh);

	mem = chars2int(buf);

	return mem;
}

int main(int argc, char** argv) {
	int mem_high = highmem();
	int mem_low = lowmem();

	int mem_total = mem_high + mem_low;
	
	printf("Memory:\n\tLow (under 640k): %dk\n\tHigh (above 1M): %dk\n\tTotal: %dk\n\n", mem_low, mem_high, mem_total);

	return 0;
}
