#include "stdio.h"
#include "syscall.h"
#include "math.h"

#include <varargs.h>

#define FORMAT_MARK '%'

void vprintf(char* text, va_list args);

int stdout, stdin, stderr;

void printi(int num, int base) {
    char buffer[64];
    char* ptr = &buffer[sizeof(buffer)-1];
    int remainder;
    
    if(base == 0) {
    	printf("Cannot have a base of 0!\n");
    	return;
    }
    
    *ptr = '\0';

    if (num == 0) {
        putc('0');
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
        putc(*ptr++);
    }
}

int print(char* str) {
	return write(stdout, str, strlen(str));
}


int putc(char c) {
	char buf[2];

	if(c == '\n')
		putc('\r');
	
	buf[1] = 0;
	buf[0] = c;
	
	return write(stdout, buf, 1);
}


int cls() {
	putc('\xC');
}


char getch() {
	return fgetch(stdin);
}


char fgetch(int fh) {
	char out[1];
	
	read(fh, &out, 1);
	
	return out[0];
}

int freadline(int fh, char* buffer) {
	char* buffer_head = buffer;
	char c;
	
	while((c=fgetch(fh)) != '\r') {
		*(buffer++) = c;
	}
	
	*(buffer++) = 0; // null-terminate
	return buffer-buffer_head;
}


int readline(char* buffer) {
	char* buffer_head = buffer;
	char c;
	
	while((c=getch()) != '\r') {
		if(c == 0x8 && buffer != buffer_head) { // backspace
			*(buffer--) = ' ';
			
			putc(' '); // Overwrite with space
			putc('\x2'); // Move cursor back
		} else {
			*(buffer++) = c;
		}
	}
	
	*(buffer++) = 0; // null-terminate
	
	return buffer-buffer_head;
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
                    printi(va_arg(args, int), 10);
                    break;

                case 'c': // char
                    putc(va_arg(args, char));
                    break;

                case 's': // string
                    print(va_arg(args, char*));
                    break;

                case 'x': // hex int
                    printi(va_arg(args, int), 16);
                    break;

                case FORMAT_MARK:
                    putc(FORMAT_MARK);
                    break;

                default:
                    break;
            }
            
            skip_next = TRUE;
        } else {
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
