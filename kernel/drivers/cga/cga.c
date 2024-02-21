#include "cga.h"
#include "util.h"
#include "type.h"

int cga_disable_cursor() {
    outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
    return 0;
}

int cga_set_cursor(char x, char y) {
    uint16_t pos = y * CGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    return 0;
}

int cga_setc(int x, int y, char c, char colour) {
    return _cga_setc(CGA_BUF(x, y), c, colour);
}

int cga_clear(char fg, char bg) {
    for(int pos=0 ; pos<CGA_BUF(CGA_WIDTH, CGA_HEIGHT) ; pos++)
        _vga_setc(pos, ' ', CGA_COLOUR(fg, bg));
    return 0;
}

int cga_get_height() {
    return CGA_HEIGHT;
}

int cga_get_width() {
    return CGA_WIDTH;
}
