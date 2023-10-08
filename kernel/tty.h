#ifndef RHYSOS_TTY_H
#define RHYSOS_TTY_H

enum {
    GRAPHICS_MONO_80x25  = 3,
    GRAPHICS_MONO_40x25  = 1,
    GRAPHICS_CGA_320x200 = 4
} GraphicsMode;

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
char getch();

void set_graphics_mode(int mode);
int get_graphics_mode();
void cls();

#endif
