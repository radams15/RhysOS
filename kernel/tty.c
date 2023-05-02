#include "tty.h"

int print_code = 0x0E;
int set_cursor_code = 0x0200;
int zero = 0x00;

int graphics_mode = GRAPHICS_CGA_80x25; // CGA 80x24

int interrupt (int number, int AX, int BX, int CX, int DX);


int get_cursor() {
	#asm
		xor ax, ax
		mov ah, 0x02
		xor bh, bh ; display page = 0
		int 0x10
		mov ax, dx
	#endasm
}

void set_cursor(char row, char col) {
	interrupt(0x10, 0x0200, 0 /*Display page 0*/, 0, 0x0C23);
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

void print_hex_8(unsigned int n) { // TODO Broken
    print_hex_4(n >> 12);
    print_hex_4(n & 4095);
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
