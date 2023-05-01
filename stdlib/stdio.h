#ifndef STDIO_H
#define STDIO_H

#include "stddef.h"

enum {
	GRAPHICS_CGA_80x25 = 0,
	GRAPHICS_CGA_40x25 = 1,
} GraphicsMode;

void printf(char* text, ...);

char getch();
int print(char* str);
int putc(char c);
int readline(char* buffer);

int set_graphics_mode(int mode);
int cls();

#endif
