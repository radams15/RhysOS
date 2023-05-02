#include "syscall.h"

int syscall(int ax, int bx, int cx, int dx) {
	int out;
	
	out = ax;
	
	interrupt(0x21, &out, bx, cx, dx);
	
	return out;
}

int exec(char* file, int argc, char** argv) {
	return syscall(3, file, argc, argv);
}

int sys_set_graphics_mode(int mode) {
	return syscall(4, mode, 0, 0);
}

int dir_list(char* dir_name, struct File* buf) {
	return syscall(5, dir_name, buf, 0);
}

int read(int fh, unsigned char* buffer, unsigned int size) {
	return syscall(6, fh, buffer, size);
}

int write(int fh, unsigned char* buffer, unsigned int size) {
	return syscall(7, fh, buffer, size);
}

int open(char* name) {
	return syscall(8, name, 0, 0);
}

void close(int fh) {
	return syscall(9, fh, 0, 0);
}

void seek(int fh, unsigned int location) {
	return syscall(10, fh, location, 0);
}
