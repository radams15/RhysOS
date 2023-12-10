#include "tty.h"

#define TAB_SIZE 4

int graphics_mode = GRAPHICS_MONO_80x25;
int font = FONT_8x16;

char text_bg = 0xF;
char text_fg = 0x0;

int interrupt(int number, int AX, int BX, int CX, int DX);

void set_cursor(char row, char col) {
    interrupt(0x10, 0x0200, 0, 0, (row << 2) | (col & 0xFF));
}

char get_cursor_row() {
    return (get_cursor() >> 2) & 0xFF;
}

char get_cursor_col() {
    return get_cursor() & 0xFF;
}

void set_bg(char colour) {
    text_bg = colour;
}

void set_fg(char colour) {
    text_fg = colour;
}

void set_resolution(int mode) {
    interrupt(0x10, mode, 0, 0, 0);
}

void print_char(int c) {
    print_char_colour(
        c, text_fg,
        text_bg);  // https://en.wikipedia.org/wiki/BIOS_color_attributes
}

void print_char_colour(int c, char fg, char bg) {
    int colour;

    colour = (fg << 2) | (bg & 0xF);

    if (c == '\t') {
        int col = get_cursor_col();
        for (int i = 0; i < col % TAB_SIZE; i++)  // Round to nearest tab col
            print_char(' ');
        return;
    }

    if (c == '\xC') {
        cls();
        return;
    }

    if (c == '\x1') {  // Cursor down
        set_cursor(get_cursor_row() - 1, get_cursor_col());
        return;
    }

    if (c == '\n')
        print_char('\r');

    if (c > 13)  // Only chars above 13 need colour
        interrupt(0x10, 0x0900 + c, colour, 1, 0);

    interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void print_string(char* str) {
    char* c;
    for (c = str; *c != 0; ++c)
        print_char(*c);
}

void printi(unsigned int num, int base) {
    char buffer[64];
    char* ptr = &buffer[sizeof(buffer) - 1];
    int remainder;

    *ptr = '\0';

    if (num == 0) {
        print_char('0');
        return;
    }

    while (num != 0) {
        remainder = num % base;

        if (remainder < 10)
            *--ptr = '0' + remainder;
        else
            *--ptr = 'A' + remainder - 10;

        num /= base;
    }

    while (*ptr != '\0') {
        print_char(*ptr++);
    }
}

int readline(char* buffer) {
    char c;

    while ((c = getch()) != '\r') {
        if (c == 0x8) {  // backspace
            *(buffer--) = ' ';
        } else {
            *(buffer++) = c;
        }
    }

    *(buffer++) = 0;  // null-terminate
}

// Getch without echo
char ngetch() {
    char out;

    out = interrupt(0x16, 0, 0, 0, 0);

    return out;
}

char getch() {
    char out;

    out = interrupt(0x16, 0, 0, 0, 0);
    print_char(out);  // echo back char

    return out;
}


void set_graphics_mode(int mode, int fnt) {
    graphics_mode = mode;
    font = fnt;

    cls();
}

int get_graphics_mode() {
    return graphics_mode;
}

void cls() {
    interrupt(0x10, (0x11 << 8) + font, 0, 0, 0);
    interrupt(0x10, graphics_mode, 0, 0,
          0);  // TODO: Replace with screen scrolling
}
