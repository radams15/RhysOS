#include <stdio.h>
#include <syscall.h>
#include <string.h>
#include <syscall.h>

typedef struct Ctx {
    int com1;
} Ctx_t;

int fb_putp(int es, int di, int ax);

int cga_putp(int x, int y, int colour) {
    return fp_putb(0xB000, 320*y + x, colour);
}

int vga_putp(int x, int y, int colour) {
    return fp_putb(0xA000, 320*y + x, colour);
}

int mainloop(Ctx_t* ctx) {
    for(int x=0 ; x<320 ; x++)
        for(int y=0 ; y<4 ; y++)
            vga_putp(x, y, 0xAA);

    getch();

    return 0;
}

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


int main() {
    int before_mode = get_graphics_mode();

    set_graphics_mode(0x13);

    int com1 = open("/dev/com1", NULL);

    Ctx_t ctx = {
        com1
    };

    int err = mainloop(&ctx);

    close(com1);

    set_graphics_mode(before_mode);

    return 0;
}
