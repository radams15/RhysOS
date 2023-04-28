#include "syscall.h"

int print(char* str) {
	return SYSCALL(0, str, 0, 0);
}

int putc(char* str) {
	return SYSCALL(1, str, 0, 0);
}

int readline(char* buffer) {
	return SYSCALL(2, buffer, 0, 0);
}

int exec(char* file) {
	return SYSCALL(3, file, 0, 0);
}

int sys_set_graphics_mode(int mode) {
	return SYSCALL(4, mode, 0, 0);
}
