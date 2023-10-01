#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");
    printf("Hello again!\n");
    
    int out1 = open("/dev/stdout");
    
    printf("stdout: %d\nOut1: %d\n", stdout, out1);
    write(out1, "TESTMSG", 7);
    return 0;
}
