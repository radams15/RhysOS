#ifndef STDIO_H
#define STDIO_H

#define NULL ((void*) 0)

extern int interrupt (int number, int AX, int BX, int CX, int DX);

int print(char* str);
int readline(char* buffer);
int exec(char* file);

char* strtok(register char* s, register char* delim);

#endif
