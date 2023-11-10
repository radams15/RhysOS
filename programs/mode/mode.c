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

void set_graphics_mode(char mode, char font) {
    int fh;

    char buf[2] = {mode, font};

    fh = open("/dev/graphmode");
    write(fh, &buf, sizeof(&buf));
    close(fh);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Current mode: %d\n", get_graphics_mode());
        return 1;
    } else if (argc == 2 || argc == 3) {
        int font = 0;
        if (argc == 3) {
            if (STREQ("8", argv[2])) {
                font = FONT_8x8;
            } else if (STREQ("16", argv[2])) {
                font = FONT_8x16;
            }
        }

        if (STREQ("40", argv[1])) {
            set_graphics_mode(GRAPHICS_MONO_40x25, font);
        } else if (STREQ("80", argv[1])) {
            set_graphics_mode(GRAPHICS_MONO_80x25, font);
        } else if (STREQ("cga", argv[1])) {
            set_graphics_mode(GRAPHICS_CGA_320x200, font);
        } else {
            printf("Unknown graphics mode: %s\n", argv[1]);
        }
    } else {
        printf("Usage: %s [MODE] (FONT)\n", argv[0]);
        return 1;
    }

    return 0;
}
