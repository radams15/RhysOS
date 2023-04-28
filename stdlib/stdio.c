#include "stdio.h"
#include "syscall.h"

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
                    //puti(va_arg(args, int));
                    break;

                case 'c': // char
                    putc(va_arg(args, char));
                    break;

                case 's': // string
                    print(va_arg(args, char*));
                    break;

                case 'x': // hex int
                    print("");
                    //print_hex(va_arg(args, int));
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
