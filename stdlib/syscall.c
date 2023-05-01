#include "syscall.h"

int readline(char* buffer) {
	return SYSCALL(2, buffer, 0, 0);
}

int exec(char* file, int argc, char** argv) {
	return SYSCALL(3, file, argc, argv);
}

int sys_set_graphics_mode(int mode) {
	return SYSCALL(4, mode, 0, 0);
}

int dir_list(char* dir_name, struct File* buf) {
	return SYSCALL(5, dir_name, buf, 0);
}

int file_read(char* buf, int n, char* file_name) {
	return SYSCALL(6, buf, n, file_name);
}

int file_write(char* buf, int n, char* file_name) {
	return SYSCALL(7, buf, n, file_name);
}

int print(char* str) {
	return file_write(str, strlen(str), "/dev/stdout");
}

int putc(char c) {
	char buf[2];
	buf[1] = 0;
	buf[0] = c;
	
	return file_write(buf, 1, "/dev/stdout");
}
