#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define TOPBAR_COLOUR 2
#define BG_COLOUR 1
#define FG_COLOUR 2

int get_graphics_mode() {
    char buf[2];
    int fh;
    int mode;

    fh = open("/dev/graphmode");

    read(fh, &buf, sizeof(buf));
    mode = chars2int(buf);

    close(fh);

    return mode;
}

void set_graphics_mode(int mode) {
    char buf[2];
    int fh;

    int2chars(mode, buf);

    fh = open("/dev/graphmode");

    write(fh, &buf, sizeof(buf));

    close(fh);
}

int graphics_set_bg(char bg) {
    return interrupt_10((0x0B << 8), (0x00 << 8) + bg, 0, 0);
}

int graphics_set_palette(char palette) {
    return interrupt_10((0x0B << 8), (0x01 << 8) + palette, 0, 0);
}

int topbar() {
    term_set_bg(TOPBAR_COLOUR);

    for (int x = 0; x < 80; x++) {  // Draw top bar
        for (int y = 0; y < 2; y++) {
            putc('O');
        }
    }
}

int draw_text(int fd) {
    term_set_bg(BG_COLOUR);
    term_set_fg(FG_COLOUR);

    seek(fd, 0);

    char buffer[32];
    for (int i = 0; i < 2; i++) {
        read(fd, &buffer, 32);
        printf("%s\n", buffer);
    }
}

int mainloop(char* fname) {
    int fd;

    fd = open(fname);
    if (fd == -1)
        return 1;

    graphics_set_palette(2);
    graphics_set_bg(BG_COLOUR);

    // while(1) {
    topbar();
    draw_text(fd);
    // }

    close(fd);

    return 0;
}

int main(int argc, char** argv) {
    int before_mode;
    int err;

    before_mode = get_graphics_mode();

    set_graphics_mode(GRAPHICS_COLOUR_80x25);

    err = mainloop("/test.bat");

    getch();

    set_graphics_mode(before_mode);

    if (err)
        fprintf(stderr, "Failed to open file!\n");

    return 0;
}
