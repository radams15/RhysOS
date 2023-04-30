#include "stdio.h"
#include "syscall.h"
#include "math.h"

#include <varargs.h>

#define FORMAT_MARK '%'

void vprintf(char* text, va_list args);

static int graphics_mode = GRAPHICS_CGA_80x25;

int set_graphics_mode(int mode) {
	graphics_mode = mode;
	cls();
}

int cls() {
	return sys_set_graphics_mode(graphics_mode);
}

void print_hex_1(unsigned int n) {
    if (n < 10)
        putc(n + '0');
    else
        putc(n - 10 + 'A');
}

void print_hex_2(unsigned int n) {
    print_hex_1(n >> 4);
    print_hex_1(n & 15);
}

void print_hex_4(unsigned int n) {
    print_hex_2(n >> 8);
    print_hex_2(n & 255);
}

void vprintf(register char* text, register va_list args) {
	BOOL skip_next = FALSE;
	int i;
	char formatter;

    for (i=0; text[i] != NULL; i++){
        if(skip_next){
            skip_next = FALSE;
            continue;
        }

        if(text[i] == FORMAT_MARK){
            formatter = text[i+1];

            switch(formatter){
                case 'd': // int
                	print("%d undefined!\n");
                    //printi(va_arg(args, int), 10);
                    break;

                case 'c': // char
                    putc(va_arg(args, char));
                    break;

                case 's': // string
                    print(va_arg(args, char*));
                    break;

                case 'x': // hex int
                    print_hex_4(va_arg(args, int));
                    break;

                case FORMAT_MARK:
                    putc(FORMAT_MARK);
                    break;

                default:
                    break;
            }

            skip_next = TRUE;

        }else{
            putc(text[i]);
        }
    }
}

void printf(char* text, va_list va_alist) {
	va_list ptr;
	va_start(ptr);
	
	vprintf(text, ptr);
	
	va_end(ptr);
}

int __mkargv() {}
