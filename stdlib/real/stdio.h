#ifndef STDIO_H
#define STDIO_H

#include "stddef.h"

enum {
    GRAPHICS_MONO_80x25 = 0x3,
    GRAPHICS_MONO_40x25 = 0x1,
    GRAPHICS_COLOUR_80x25 = 0x3,
    GRAPHICS_CGA_320x200 = 0x4
} GraphicsMode;

enum { FONT_8x8 = 0x12, FONT_8x16 = 0x00 } Font;

typedef struct File {
    int fh;
} File_t;

extern int stdout, stdin, stderr;

void fprintf(int fd, char* text, ...);
void printf(char* text, ...);

char ngetch();
char getch();
char fgetch(int fh);
int fprint(int fh, char* str);
int print(char* str);
int putc(char c);
int fputc(int fh, char c);
int readline(char* buffer);
int freadline(int fh, char* buffer);
void exit(char code);  // Defined in crt0.c

File_t* fopen(const char* fname, const char* mode);
int fwrite(File_t* file, unsigned char* buffer, unsigned int size);
int fclose(File_t* file);
int fread(File_t* file, unsigned char* buffer, unsigned int size);
int fseek(File_t* file, int pos);

void term_set_bg(char colour);
void term_set_fg(char colour);

int cls();

#endif
