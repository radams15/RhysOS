#ifndef VGA_H
#define VGA_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 24

#define VGA_BUF(x, y) ((y * VGA_WIDTH) + x)
#define VGA_COLOUR(fg, bg) (fg * 0x10) | (bg)

int _vga_setc(int pos, char c, char colour);
int vga_setc(int x, int y, char c, char colour);
int vga_scroll();
int vga_set_cursor(char x, char y);
int vga_disable_cursor();
int vga_clear(char fg, char bg);

int vga_get_height();
int vga_get_width();

#endif
