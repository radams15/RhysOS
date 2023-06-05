#ifndef STDIO_H
#define STDIO_H

#include "stddef.h"

enum {
	GRAPHICS_MONO_80x25 = 3,
	GRAPHICS_MONO_40x25 = 1,
	GRAPHICS_CGA_320x200 = 4
} GraphicsMode;

extern int stdout, stdin, stderr;

void printf(char* text, ...);

char getch();
char fgetch(int fh);
int print(char* str);
int putc(char c);
int readline(char* buffer);
int freadline(int fh, char* buffer);
void exit(char code); // Defined in crt0.c

int cls();

#endif
