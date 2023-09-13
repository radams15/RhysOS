#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define MAX_FILES 64

void dir_listing(char* dir) {
    int i;
    int len;
    FsNode_t dir_buf[MAX_FILES];
    FsNode_t* file;

    for (i = 0; i < MAX_FILES; i++) {
        dir_buf[i].name[0] = 0;
    }

    printf("Files in directory '%s'\n", dir);
    len = dir_list(dir, dir_buf, MAX_FILES);
    
    printf("There are %d files\n", len);

    for (i = 0; i < len; i++) {
        file = &dir_buf[i];

        if (file == NULL || file->name[0] == 0)
            continue;

        printf("\t - %s\n", file->name);
    }
}

int main(int argc, char** argv) {
    int i;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            dir_listing(argv[i]);
        }
    } else {
        dir_listing("/");
    }

    return 0;
}
