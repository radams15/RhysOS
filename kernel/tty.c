#include "tty.h"

int print_code = 0x0E;
int set_cursor_code = 0x0200;
int zero = 0x00;
char hex_chars[] = "0123456789ABCDEF";

int graphics_mode = GRAPHICS_CGA_80x25; // CGA 80x24

int interrupt (int number, int AX, int BX, int CX, int DX);

void clear_screen() {
	interrupt(0x10, 0x0200, 0, 0, 0);
	interrupt(0x10, 0x0600, 0x0f00, 0, 0x184f);
}

void set_resolution(int mode) {
	interrupt(0x10, mode, 0, 0, 0);
}

void print_char(int c) {
	if(c == '\t') {
		print_string("    ");
		return;
	}

	if(c == '\n')
		interrupt(0x10, 0x0E00 + '\r', 0, 0, 0);
	
	interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void print_string(char* str) {
    char* c;
    for(c=str ; *c != 0 ; ++c)
        print_char(*c);
}

void print_hex(int n) { // TODO Improve this to handle > 3 bytes.
    print_string("0x");
    print_char(hex_chars[(n & 0xF00)>>8]);
    print_char(hex_chars[(n & 0xF0)>>4]);
    print_char(hex_chars[(n & 0xF)>>0]);
}

int readline(char* buffer) {
	char c;
	
	while((c=getch()) != '\r') {
		if(c == 0x8) { // backspace
			*(buffer--) = ' ';
		} else {
			*(buffer++) = c;
		}
	}
	
	*(buffer++) = 0; // null-terminate
}

char getch() {
	char out;
	
	out = interrupt(0x16, 0, 0, 0, 0);
	print_char(out); // echo back char
	
	return out;
}

int get_graphics_ah(int mode) {
	switch(mode) {
		case GRAPHICS_CGA_80x25:
			return 0x03;
		
		case GRAPHICS_CGA_40x25:
			return 0x00;
			
		default:
			return 0x03;
	}
}

void set_graphics_mode(int mode) {
	graphics_mode = mode;
	
	cls();
}

void cls() {
	interrupt(0x10, get_graphics_ah(graphics_mode), 0, 0, 0);
}
