#include <stdio.h>

#define TESTS 10

int main(int argc, char** argv) {
    printf("Hello world!\n");
    printf("Hello again!\n");

    int out1 = open("/dev/stdout", NULL);
    int rand = open("/dev/rand", NULL);

    printf("stdout: %d\nOut1: %d\nRand: %d\n", stdout, out1, rand);
    write(out1, "TESTMSG\n", 8);

    int rands[TESTS];
    for(int i=0 ; i<TESTS ; i++) {
        read(rand, &rands[i], sizeof(int));
        rands[i] %= 500;
    }

    char* ptrs[TESTS];
    for (int i = 0; i < TESTS; i++) {
        ptrs[i] = malloc(rands[i]);
        printf("Malloc %d: %x\n", rands[i], ptrs[i]);

        if (ptrs[i] == 0)
            goto fail;
    }

    for (int i = 0; i < TESTS; i++) {
        free(ptrs[i]);
    }

    printf("Pass phase 1\n");

    void* a;
    for (int i = 0; i < TESTS; i++) {
        printf("Malloc %d", i);
        a = malloc(25);
        if (a == 0)
            goto fail;
        free(a);
    }

    close(rand);
    close(out1);
end:
    return 0;

fail:
    fprintf(stderr, "Failed tests!");
}
