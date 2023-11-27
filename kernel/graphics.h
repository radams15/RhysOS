#ifndef GRAPHICS_H
#define GRAPHICS_H

void g_putp(int x, int y, int colour);
int g_set_bg(char bg);
int g_set_palette(char palette);
int cga_putp(int x, int y, int colour);
int vga_putp(int x, int y, int colour);
int fp_putb(int es, int di, int val);


#endif
