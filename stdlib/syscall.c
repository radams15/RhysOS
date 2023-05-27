#include "syscall.h"

extern int stdin, stdout, stderr;

int syscall(int ax, int bx, int cx, int a, int b, int c, int d) {
	int out;
	
	int extra[4];
	extra[0] = a;
	extra[1] = b;
	extra[2] = c;
	extra[3] = d;
	
	out = ax;
	
	interrupt(0x21, &out, bx, cx, &extra);
	
	return out;
}

int execa(char* file, int argc, char** argv, int in, int out, int err) {
	return syscall(3, file, argc, argv, in, out, err);
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



void int2chars(unsigned int in, unsigned char* buffer) {
	buffer[0] = (unsigned char) in & 0xff; // low byte
	buffer[1] = (in >> 8) & 0xff; // high byte
}

int chars2int(unsigned char* buffer) {
	return (buffer[1] << 8) | buffer[0];
}
