#include <stdio.h>
#include <string.h>
#include <syscall.h>

extern void enter_pmode();

int main(int argc, char** argv) {
    enter_pmode();

    printf("Done!\n");

    return 0;
}
