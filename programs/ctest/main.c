#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");
    printf("Hello again!\n");
    write(stdout, "TESTMSG", 7);
    return 0;
}
