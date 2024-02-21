#include "vga.h"
#include "util.h"
#include "type.h"

void vga_disable_cursor() {
    outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void vga_set_cursor(char x, char y) {
    uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
