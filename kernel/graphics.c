#include "graphics.h"

#include "interrupt.h"

int g_set_bg(char bg) {
	return interrupt_10((0x0B<<8), (0x00<<8) + bg, 0, 0);
}

int g_set_palette(char palette) {
	return interrupt_10((0x0B<<8), (0x01<<8) + palette, 0, 0);
}


void g_putp(int x, int y, int colour) {
   	return interrupt_10((0x0C<<8) + colour, x, y, 0); 
}

int cga_putp(int x, int y, int colour) {
    return fp_putb(0xB000, 320*y + x, colour);
}

int vga_putp(int x, int y, int colour) {
    return fp_putb(0xA000, 320*y + x, colour);
}
