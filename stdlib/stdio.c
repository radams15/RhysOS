#include "stdio.h"

#define syscall(bx, cx, dx) interrupt(0x21, bx, cx, dx)

int print(char* str) {
	return syscall(0, str, 0, 0);
}

int readline(char* buffer) {
	return syscall(1, buffer, 0, 0);
}

int exec(char* file) {
	return syscall(2, file, 0, 0);
} 
