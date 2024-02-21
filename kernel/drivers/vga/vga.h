#ifndef VGA_H
#define VGA_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 24

#define VGA_BUF(x, y) ((y*VGA_WIDTH) + x)
#define VGA_COLOUR(fg, bg) (fg*0x10) | (bg)

int vga_setc(int pos, char c, char colour);
int vga_scroll();
void vga_set_cursor(char x, char y);
void vga_disable_cursor();

#endif
