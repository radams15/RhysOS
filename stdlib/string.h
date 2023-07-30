#ifndef STRING_H
#define STRING_H

char* strtok(register char* s, register char* delim);

int strlen(char* str);
int strcmp(char* a, char* b);
int strncmp(char* a, char* b, int length);
int endswith(char* a, char* suffix);

#define STREQ(a, b) (strcmp(a, b) == 0)

#endif
