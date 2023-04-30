#ifndef RHYSOS_UTIL_H
#define RHYSOS_UTIL_H

void memset(int *dest, int val, int len);

int div (int a, int b);

int mod (int a, int n);

void memcpy(char *dest, char *src, unsigned int n);

void clear(int *ptr, unsigned int n);

int strlen(char* str); 

int strncmp(char* a, char*b, int length);

int strcmp(char* a, char* b);

int oct2bin(unsigned char* str, int size);

#endif
