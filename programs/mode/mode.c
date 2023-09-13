#include <stdio.h>
#include <string.h>
#include <syscall.h>

int get_graphics_mode() {
    int fh;

    char buf[2];
    fh = open("/dev/graphmode");
    read(fh, &buf, sizeof(&buf));
    close(fh);

    return chars2int(buf);
}

void set_graphics_mode(int mode) {
    int fh;

    char buf[2];
    int2chars(mode, &buf);

    fh = open("/dev/graphmode");
    write(fh, &buf, sizeof(&buf));
    close(fh);
}

int main(int argc, char** argv) {
    printf("Argv: %x\n", argv);

    for(int i=0 ; i<argc ; i++) {
        printf("Arg %d = %s\n", i, argv[i]);
    }
    return 0;
    

    if (argc == 1) {
        printf("Current mode: %d\n", get_graphics_mode());
    } else if (argc == 2) {
        if (STREQ("40", argv[1])) {
            set_graphics_mode(GRAPHICS_MONO_40x25);
        } else if (STREQ("cga", argv[1])) {
            set_graphics_mode(GRAPHICS_CGA_320x200);
        } else {
            printf("Unknown graphics mode: %s\n", argv[1]);
        }
    } else {
        printf("Usage: %s [MODE]\n", argv[0]);
        return 1;
    }

    return 0;
}
