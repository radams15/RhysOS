#ifndef CGA_H
#define CGA_H

#define CGA_WIDTH 80
#define CGA_HEIGHT 24

#define CGA_BUF(x, y) ((y*CGA_WIDTH) + x)
#define CGA_COLOUR(fg, bg) (fg*0x10) | (bg)

int _cga_setc(int pos, char c, char colour);
int cga_setc(int x, int y, char c, char colour);
int cga_scroll();
int cga_set_cursor(char x, char y);
int cga_disable_cursor();
int cga_clear(char fg, char bg);

int cga_get_height();
int cga_get_width();

#endif
