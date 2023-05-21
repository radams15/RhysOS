#include "tty.h"

int graphics_mode = GRAPHICS_CGA_80x25; // CGA 80x24

int intr(int number, int AX, int BX, int CX, int DX);

void set_cursor(char row, char col) {
	intr(0x10, 0x02<<8, 0, 0, (row<<8) | col);
}

void set_resolution(int mode) {
	intr(0x10, mode, 0, 0, 0);
}

void print_char(char c) {
	if(c == '\t') {
		print_string("    ");
		return;
	}
	
	if(c == '\xC') {
		cls();
		return;
	}

	if(c == '\x1') { // Cursor down
		set_cursor(get_cursor_row()-1, get_cursor_col());
		return;
	}
	
	if(c == '\x2') { // Cursor back
		char col = get_cursor_col();
		set_cursor(get_cursor_row(), col>0? col-1 : col);
		return;
	}


	if(c == '\n')
		intr(0x10, 0x0E00 + '\r', 0, 0, 0);

	intr(0x10, 0x0E00 + c, 0, 0, 0);
}

void print_string(char* str) {
    char* c;
    for(c=str ; *c != 0 ; ++c)
        print_char(*c);
}

void print_hex_1(unsigned int n) {
    if (n < 10)
        print_char(n + '0');
    else
        print_char(n - 10 + 'A');
}

void print_hex_2(unsigned int n) {
    print_hex_1(n >> 4);
    print_hex_1(n & 15);
}

void print_hex_4(unsigned int n) {
    print_hex_2(n >> 8);
    print_hex_2(n & 255);
}

void printi(int num, int base) {
    char buffer[64];
    char* ptr = &buffer[sizeof(buffer)-1];
    int remainder;
    
    *ptr = '\0';

    if (num == 0) {
        print_char('0');
        return;
    }

    while (num != 0) {
        remainder = imod(num, base);
        
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
	
	out = intr(0x16, 0, 0, 0, 0);
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

int get_graphics_mode() {
	return graphics_mode;
}

void cls() {
	intr(0x10, get_graphics_ah(graphics_mode), 0, 0, 0);
}
