#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define TOPBAR_COLOUR 2
#define BG_COLOUR 1
#define FG_COLOUR 2

char buffer[4096];
int yval = 0;

int get_graphics_mode() {
    char buf[2];
    int fh;
    int mode;

    fh = open("/dev/graphmode", NULL);

    read(fh, &buf, sizeof(buf));
    mode = chars2int(buf);

    close(fh);

    return mode;
}

void set_graphics_mode(int mode) {
    char buf[2];
    int fh;

    int2chars(mode, buf);

    fh = open("/dev/graphmode", NULL);

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

char* next_line(char* str) {
    char* begin;

    for (begin = str; *begin != '\n' && *begin != 0; begin++) {
    }

    begin++;

    return begin;
}

char* prev_line(char* str) {
    char* begin;

    for (begin = str; *begin != '\n' && *begin != 0; begin--) {
    }

    return begin;
}

void scroll_down(int n) {
    yval += n;

    if (yval > 24)
        yval = 24;
}

void scroll_up(int n) {
    yval -= n;

    if (yval <= 0)
        yval = 0;
}

int handle_key(char c) {
    switch (c) {
        case 'n':
            scroll_down(1);
            break;

        case 'p':
            scroll_up(1);
            break;

        case 'q':
            return 1;
    }

    return 0;
}

int redraw(int from, int to) {
    char* line = buffer;

    int i;
    for (i = 0; i < from; i++)
        line = next_line(line);

    for (i = from; i < to; i++) {
        line = next_line(line);

        for (char* c = line; *c != '\n' && *c != 0; c++)
            putc(*c);
        putc('\n');
    }
}

int begin(char* fname) {
    int fd;

    fd = open(fname, NULL);
    if (fd == -1)
        return 1;

    read(fd, &buffer, sizeof(buffer));

    close(fd);

    // graphics_set_palette(2);
    // graphics_set_bg(BG_COLOUR);
    // term_set_bg(BG_COLOUR);
    // term_set_fg(FG_COLOUR);

    char c;

    do {
        redraw(yval, yval + 24);

        c = ngetch() & 0xFF;
    } while (handle_key(c) == 0);

    return 0;
}

int main(int argc, char** argv) {
    int before_mode;
    int err;

    before_mode = get_graphics_mode();

    set_graphics_mode(GRAPHICS_COLOUR_80x25);

    err = begin("/syscalls.md");

    set_graphics_mode(before_mode);

    if (err)
        fprintf(stderr, "Failed to open file!\n");

    return 0;
}
