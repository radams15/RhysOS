#ifndef RHYSOS_UTIL_H
#define RHYSOS_UTIL_H

void memset(int* dest, int val, int len);

#define strcpy(a, b) memcpy(a, b, strlen(b))
#define strcpyz(a, b) \
    strcpy(a, b);     \
    a[strlen(b)] = 0

void memcpy(char* dest, char* src, unsigned int n);

int div(int dividend, int divisor);

int mod(int dividend, int divisor);

void clear(int* ptr, unsigned int n);

int strlen(char* str);

int strncmp(char* a, char* b, int length);

int strcmp(char* a, char* b);

int oct2bin(unsigned char* str, int size);

char* strtok(char* s, char* delim);

int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

void outb(unsigned int port, unsigned char val);
unsigned char inb(unsigned int port);

#endif
