#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL(ax, bx, cx, dx) interrupt(0x21, ax, bx, cx, dx)

extern int interrupt (int number, int AX, int BX, int CX, int DX);


typedef struct File {
	char name[100];
	unsigned int sector_start;
	unsigned int size; // size in bytes.
} File_t;

typedef int (*FsCallback)(struct File*);


int print(char* str);
int readline(char* buffer);
int exec(char* file, int argc, char** argv);
int sys_set_graphics_mode(int mode);
int dir_list(char* dir_name, struct File* buf);
int file_read(char* buf, int n, char* file_name);

#endif
