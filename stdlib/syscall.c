#include "syscall.h"

extern int stdin, stdout, stderr;

extern void biosprint();
int ds();

typedef struct SyscallArgs {
  int num; // Syscall number
  int a, b, c, d, e, f;
  int ds; // Data segment
} SyscallArgs_t;

int syscall(int num, int a, int b, int c, int d, int e, int f) {
        SyscallArgs_t out = {
          num,
          a, b,c,d,e,f,
          ds(),
        };

	interrupt_21(&out, 0, 0, 0);
	
	return out.num;
}

int execa(char* file, int argc, char** argv, int in, int out, int err) {
	return syscall(1, file, argc, argv, in, out, err);
}

int dir_list(char* dir_name, struct FsNode* buf, int max) {
	return syscall(2, dir_name, buf, max, 0, 0, 0);
}

int read(int fh, unsigned char* buffer, unsigned int size) {
	return syscall(3, fh, buffer, size, 0, 0, 0);
}

int write(int fh, unsigned char* buffer, unsigned int size) {
        return syscall(4, fh, buffer, size, 0, 0, 0);
}

int open(char* name) {
	return syscall(5, name, 0, 0, 0, 0, 0);
}

void close(int fh) {
	return syscall(6, fh, 0, 0, 0, 0, 0);
}

void seek(int fh, unsigned int location) {
	return syscall(7, fh, location, 0, 0, 0, 0);
}



void int2chars(unsigned int in, unsigned char* buffer) {
	buffer[0] = (unsigned char) in & 0xff; // low byte
	buffer[1] = (in >> 8) & 0xff; // high byte
}

int chars2int(unsigned char* buffer) {
	return (buffer[1] << 8) | buffer[0];
}
