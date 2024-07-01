#ifndef STRING_H
#define STRING_H

char* strtok(register char* s, register char* delim);

int strlen(char* str);
int strcmp(char* a, char* b);
int strncmp(char* a, char* b, int length);
int strcpy(char* dst, char* src);
int strncpy(char* dst, char* src, int length);
int endswith(char* a, char* suffix);

int atoi(char* str);
int strtoi(const char* nptr, char** endptr, int base);

#define STREQ(a, b) (strcmp(a, b) == 0)

#endif
