#include <stdio.h>
#include <syscall.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s [FILE]", argv[0]);
        return 1;
    }

    Stat_t stat_t;
    int err = stat(argv[1], &stat_t);

    if(err) {
        fprintf(stderr, "Cannot open file '%s'\n", argv[1]);
        return 1;
    }

    printf("Name: \t%s\nLength:\t%d\nInode: \t%d\n", argv[1], stat_t.length, stat_t.inode);

    return 0;
}
