#include "stdio.h"
#include "math.h"
#include "syscall.h"

#include <stdarg.h>

#define FORMAT_MARK '%'

void vfprintf(int fh, char* text, va_list args);

int stdout, stdin, stderr;

void printi(int fh, int num, int base) {
    char buffer[64];
    char* ptr = &buffer[sizeof(buffer) - 1];
    int remainder;

    if (base == 0) {
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

int fprint(int fh, char* str) {
    return write(fh, str, strlen(str));
}

int print(char* str) {
    return fprint(stdout, str);
}

int fputc(int fh, char c) {
    char buf[2];

    buf[1] = 0;
    buf[0] = c;

    return write(fh, buf, 1);
}

int putc(char c) {
    char buf[2];

    buf[1] = 0;
    buf[0] = c;

    return write(stdout, buf, 1);
}

int cls() {
    putc('\xC');
}

char ngetch() {
    return fgetch(stdin);
}

char getch() {
    char c = fgetch(stdin);

    putc(c);

    return c;
}

char fgetch(int fh) {
    char out[1];

    int len = read(fh, &out, 1);

    if (len == 0)
        return 0;

    return out[0];
}

int freadline(int fh, char* buffer) {
    char* buffer_head = buffer;
    char c;

    while ((c = fgetch(fh)) != '\n') {
        *(buffer++) = c;
    }

    *(buffer++) = 0;  // null-terminate
    return buffer - buffer_head;
}

int readline(char* buffer) {
    const char* buffer_head = buffer;
    char c;
    int len = 0;

    while ((c = getch()) != '\n') {
        if (c == 0x8) {  // backspace
            if(buffer == buffer_head) {
                putc(' ');
                continue;
            }

            len--;
            *(buffer--) = ' ';

            putc(' ');  // Overwrite with space
            putc(0x8);  // Move cursor back
        } else {
            len++;
            *(buffer++) = c;
        }
    }

    *buffer = 0;  // null-terminate, overwrite \n

    return len;
}

void vfprintf(int fh, register char* text, register va_list args) {
    BOOL skip_next = FALSE;
    int i;
    char formatter;

    for (i = 0; text[i] != NULL; i++) {
        if (skip_next) {
            skip_next = FALSE;
            continue;
        }

        if (text[i] == FORMAT_MARK) {
            formatter = text[i + 1];

            switch (formatter) {
                case 'd':  // int
                    printi(fh, va_arg(args, int), 10);
                    break;

                case 'c':  // char
                    fputc(fh, va_arg(args, char));
                    break;

                case 's':  // string
                    fprint(fh, va_arg(args, char*));
                    break;

                case 'x':  // hex int
                    printi(fh, va_arg(args, int), 16);
                    break;

                case FORMAT_MARK:
                    fputc(fh, FORMAT_MARK);
                    break;

                default:
                    break;
            }

            skip_next = TRUE;
        } else {
            fputc(fh, text[i]);
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

    fh = open("/dev/ttybg", NULL);

    write(fh, &colour, 1);

    close(fh);
}

void term_set_fg(char colour) {
    int fh;

    fh = open("/dev/ttyfg", NULL);

    write(fh, &colour, 1);

    close(fh);
}

File_t* fopen(const char* fname, const char* mode) {
    FileMode_t mode_val = O_RDONLY;

    int fh = open(fname, mode_val);

    for (char* c = mode; *c != NULL; c++) {
        if (*c == 'w')
            mode_val |= O_CREAT;
        mode_val &= ~O_RDONLY;
        if (*c == 'a')
            mode_val |= O_APPEND;
    }

    if (fh == -1) {
        return NULL;
    }

    File_t* out = malloc(sizeof(File_t));
    out->fh = fh;

    return out;
}

int fread(File_t* file, unsigned char* buffer, unsigned int size) {
    return read(file->fh, buffer, size);
}

int fwrite(File_t* file, unsigned char* buffer, unsigned int size) {
    return write(file->fh, buffer, size);
}

int fclose(File_t* file) {
    int out = close(file->fh);

    free(file);

    return out;
}

int fseek(File_t* file, int pos) {
    return seek(file->fh, pos);
}

int __mkargv() {}
