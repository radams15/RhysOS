#include "graphics.h"

#include "tty.h"

int graphics_putc(int x, int y, int colour) {
	return interrupt(0x10, (0x0C<<8) + colour, 0, x, y);
}

int graphics_set_bg(char bg) {
	return interrupt(0x10, (0x0B<<8), (0x00<<8) + bg, 0, 0);
}

int graphics_set_palette(char palette) {
	return interrupt(0x10, (0x0B<<8), (0x01<<8) + palette, 0, 0);
}

int scroll_window() {
	interrupt(0x10, (0x06<<8), 0, 0, 0);
}

extern void graphics_test();

void graphics_init() {
	int x, y, colour;
	
	set_graphics_mode(GRAPHICS_CGA_320x200);
	
	graphics_set_palette(0);
	graphics_set_bg(3);
		
	print_string("Graphics Initialised!\n");
	
	colour = 0;
	x = 30;
	y = 100;
	while(colour <= 15) {	
		graphics_putc2(x, y, colour);
		x++;
		y++;
		colour++;
	}
}
