#include "syscall.h"

int exec(char* file, int argc, char** argv) {
	return SYSCALL(3, file, argc, argv);
}

int sys_set_graphics_mode(int mode) {
	return SYSCALL(4, mode, 0, 0);
}

int dir_list(char* dir_name, struct File* buf) {
	return SYSCALL(5, dir_name, buf, 0);
}

int read(int fh, unsigned char* buffer, unsigned int size) {
	return SYSCALL(6, fh, buffer, size);
}

int write(int fh, unsigned char* buffer, unsigned int size) {
	return SYSCALL(7, fh, buffer, size);
}

int open(char* name) {
	return SYSCALL(8, name, 0, 0);
}

void close(int fh) {
	return SYSCALL(9, fh, 0, 0);
}
