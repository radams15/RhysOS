#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");
    printf("Hello again!\n");

    int out1 = open("/dev/stdout");

    printf("stdout: %d\nOut1: %d\n", stdout, out1);
    write(out1, "TESTMSG", 7);

    void* a;
    for (int i = 0; i < 10; i++) {
        a = malloc(25);
    }

    for (int i = 0; i < 100; i++) {
        a = malloc(70);
        free(a);
    }

    return 0;
}
