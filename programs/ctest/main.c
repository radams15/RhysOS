#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");
    printf("Hello again!\n");

    int out1 = open("/dev/stdout");

    printf("stdout: %d\nOut1: %d\n", stdout, out1);
    write(out1, "TESTMSG\n", 8);

    char* ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(25);

        if (ptrs[i] == 0)
            goto end;
    }

    for (int i = 0; i < 10; i++) {
        free(ptrs[i]);
    }

    printf("Pass phase 1\n");

    void* a;
    for (int i = 0; i < 10; i++) {
        printf("Malloc %d", i);
        a = malloc(25);
        free(a);
    }

end:
    return 0;
}
