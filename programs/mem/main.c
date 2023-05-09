#include <stdio.h>

int lowmem() {
	int fh;
	int mem;

	char buf[2];
	fh = open("/dev/lowmem");
	read(fh, &buf, sizeof(&buf));
	close(fh);

	mem = (buf[1] << 8) | buf[0];

	return mem;
}

int highmem() {
	int fh;
	int mem;

	char buf[2];
	fh = open("/dev/highmem");
	read(fh, &buf, sizeof(&buf));
	close(fh);

	mem = (buf[1] << 8) | buf[0];

	return mem;
}

int main(int argc, char** argv) {
	int mem_high = highmem();
	int mem_low = lowmem();

	int mem_total = mem_high + mem_low;
	
	printf("Low (under 640k): %dk\nHigh (above 1M): %dk\nTotal: %dk\n", mem_low, mem_high, mem_total);

	return 0;
}
