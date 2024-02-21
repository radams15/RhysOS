#include "tty.h"
#include "util.h"
#include "type.h"
#include "drivers/vga/vga.h"
#include "drivers/cga/cga.h"

#define TAB_SIZE 4

int graphics_mode = GRAPHICS_MONO_80x25;
int font = FONT_8x16;
int xpos = 0;
int ypos = 0;
TTYDriver_t tty_driver;

char text_bg = 0xF;
char text_fg = 0x0;

int interrupt(int number, int AX, int BX, int CX, int DX);


char get_cursor_row() {
    return xpos;
}

char get_cursor_col() {
    return ypos;
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

void scroll() {
    tty_driver.tty_scroll_line();
    ypos--;
}

void print_char(int c) {
    print_char_colour(
        c, text_fg,
        text_bg);  // https://en.wikipedia.org/wiki/BIOS_color_attributes
}

void print_char_colour(int c, char fg, char bg) {
    if(ypos >= tty_driver.tty_get_height()) {
        scroll(); 
    }

    switch(c) {
        case '\n':
            ypos++;
            xpos = 0;
            break;

        case '\r':
            xpos = 0;
            break;

        case 0x08: // backspace
            xpos--;
            tty_driver.tty_setc(xpos, ypos, ' ', VGA_COLOUR(fg, bg));
            break;

        case '\t': {
            int col = xpos;
            for (int i = 0; i < col % TAB_SIZE; i++)  // Round to nearest tab col
                print_char_colour(' ', fg, bg);
            break;
        }

        case '\xC':
           cls();
           break;

        case '\x1':
            ypos--;
            break;

        default:
            tty_driver.tty_setc(xpos, ypos, (char) c & 0xFF, VGA_COLOUR(fg, bg));
            xpos++;
            break;
    }

    if(xpos >= tty_driver.tty_get_width()-1) {
        print_char_colour('\n', fg, bg);
    }

    tty_driver.tty_set_cursor(xpos, ypos);
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

    interrupt(0x10, (0x11 << 8) + font, 0, 0, 0);
    interrupt(0x10, graphics_mode, 0, 0,
              0);  // TODO: Replace with screen scrolling
    cls();
}

int get_graphics_mode() {
    return graphics_mode;
}

void cls() {
    xpos = 0;
    ypos = 0;

    tty_driver.tty_clear(text_fg, text_bg);
}


void graphics_init() {
    tty_driver.tty_setc = vga_setc;
    tty_driver.tty_clear = vga_clear;
    tty_driver.tty_scroll_line = vga_scroll;
    tty_driver.tty_set_cursor = vga_set_cursor;
    tty_driver.tty_get_width = vga_get_width;
    tty_driver.tty_get_height = vga_get_height;

    set_graphics_mode(graphics_mode, font);
}
