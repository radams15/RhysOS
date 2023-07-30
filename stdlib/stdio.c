#include "stdio.h"
#include "syscall.h"
#include "math.h"

#include <stdarg.h>

#define FORMAT_MARK '%'

void vfprintf(int fh, char* text, va_list args);

int stdout, stdin, stderr;

void printi(int fh, int num, int base) {
    char buffer[64];
    char* ptr = &buffer[sizeof(buffer)-1];
    int remainder;
    
    if(base == 0) {
    	fprintf(fh, "Cannot have a base of 0!\n");
    	return;
    }
    
    *ptr = '\0';

    if (num == 0) {
        fputc(fh, '0');
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
        fputc(fh, *ptr++);
    }
}

int print(char* str) {
	return write(stdout, str, strlen(str));
}

int fputc(int fh, char c) {
	char buf[2];

	if(c == '\n')
		fputc(fh, '\r');
	
	buf[1] = 0;
	buf[0] = c;
	
	return write(fh, buf, 1);
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
	int len = 0;
	
	while((c=getch()) != '\r') {
		if(c == 0x8 && buffer != buffer_head) { // backspace
		        len--;
			*(buffer--) = ' ';
			
			putc(' '); // Overwrite with space
			putc(0x8); // Move cursor back
		} else {
		        len++;
			*(buffer++) = c;
		}
	}
	
	*(buffer++) = 0; // null-terminate
	
	return len;
}

void vfprintf(int fh, register char* text, register va_list args) {
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
                    printi(fh, va_arg(args, int), 10);
                    break;

                case 'c': // char
                    fputc(fh, va_arg(args, char));
                    break;

                case 's': // string
                    print(va_arg(args, char*));
                    break;

                case 'x': // hex int
                    printi(fh, va_arg(args, int), 16);
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

void fprintf(int fd, char* text, ...) {
	va_list ptr;
	va_start(ptr, fd);
	
	vfprintf(fd, text, ptr);
	
	va_end(ptr);
}

void printf(char* text, ...) {
	va_list ptr;
	va_start(ptr, text);
	
	vfprintf(stdout, text, ptr);
	
	va_end(ptr);
}

void term_set_bg(char colour) {
	int fh;
	
	fh = open("/dev/ttybg");
	
	write(fh, &colour, 1);
	
	close(fh);
}

void term_set_fg(char colour) {
	int fh;
	
	fh = open("/dev/ttyfg");
	
	write(fh, &colour, 1);
	
	close(fh);
}

int __mkargv() {}
