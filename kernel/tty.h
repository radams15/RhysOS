#ifndef RHYSOS_TTY_H
#define RHYSOS_TTY_H

enum {
    GRAPHICS_MONO_80x25 = 0x3,
    GRAPHICS_MONO_40x25 = 0x1,
    GRAPHICS_COLOUR_80x25 = 0x3,
    GRAPHICS_CGA_320x200 = 0x4,
    GRAPHICS_VGA_640x480 = 0x12
} GraphicsMode;

enum { FONT_8x8 = 0x12, FONT_8x16 = 0x00 } Font;

void clear_screen();

void set_resolution(int mode);

void print_char(int c);
void print_char_colour(int c, char fg, char bg);

void set_bg(char colour);
void set_fg(char colour);

void print_stringn(char* str, int n);
void print_string(char* str);
void printi(unsigned int num, int base);

int get_cursor();
char get_cursor_col();
char get_cursor_row();
void set_cursor(char row, char col);

int readline(char* buffer);
char ngetch();
char getch();

void set_graphics_mode(int mode, int fnt);
int get_graphics_mode();
void cls();

void graphics_init();

#endif
