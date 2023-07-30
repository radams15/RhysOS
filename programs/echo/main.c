#include <stdio.h>
#include <string.h>
#include <syscall.h>

int main(int argc, char** argv) {
    int i;

    for (i = 1; i < argc; i++)
        printf("%s ", argv[i]);

    return 0;
}
