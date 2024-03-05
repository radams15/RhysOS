#include <stdio.h>

#include "malloc.h"

int main() {
    memmgr_init();

    unsigned char* ptrs[25];

    for(int i=0 ; i<25 ; i++) {
        ptrs[i] = kmalloc(125);
        printf("0x%x, %ld\n", ptrs[i], ptrs[i]-ptrs[i-1]);
    }

    printf("FREEING\n");
    for(int i=0 ; i<25 ; i++) {
        kfree(ptrs[i]);
    }

    for(int i=0 ; i<25 ; i++) {
        ptrs[i] = kmalloc(125);
        printf("0x%x, %ld\n", ptrs[i], ptrs[i]-ptrs[i-1]);
    }

    return 0;
}
