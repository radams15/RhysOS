#include "vga.h"
#include "type.h"
#include "util.h"

int vga_disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
    return 0;
}

int vga_set_cursor(char x, char y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    return 0;
}

int vga_setc(int x, int y, char c, char colour) {
    return _vga_setc(VGA_BUF(x, y), c, colour);
}

int vga_clear(char fg, char bg) {
    for (int pos = 0; pos < VGA_BUF(VGA_WIDTH, VGA_HEIGHT); pos++)
        _vga_setc(pos, ' ', VGA_COLOUR(fg, bg));
    return 0;
}

int vga_get_height() {
    return VGA_HEIGHT;
}

int vga_get_width() {
    return VGA_WIDTH;
}
