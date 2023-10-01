#include <stdio.h>

int lowmem() {
    int fh;
    int mem = 0;

    fh = open("/dev/lowmem");
    
    if(fh == -1) {
        fprintf(stderr, "Cannot open lowmem!\n");
        return 0;
    }
    
    read(fh, &mem, sizeof(int));
    close(fh);

    return mem;
}

int highmem() {
    int fh;
    int mem = 0;
    
    fh = open("/dev/highmem");
    
    if(fh == -1) {
        fprintf(stderr, "Cannot open highmem!\n");
        return 0;
    }

    read(fh, &mem, sizeof(int));
    close(fh);
    return mem;
}

int main(int argc, char** argv) {
    int mem_high = highmem();
    int mem_low = lowmem();

    printf("Memory:\n\t- Low (under 640k): %dk\n\t- High (above 1M): %dk\n",
           mem_low, mem_high);

    return 0;
}
