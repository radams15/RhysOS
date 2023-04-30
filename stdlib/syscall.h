#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL(ax, bx, cx, dx) interrupt(0x21, ax, bx, cx, dx)

#define FILE_NAME_MAX 128

extern int interrupt (int number, int AX, int BX, int CX, int DX);


typedef struct DirEnt {
	char name[FILE_NAME_MAX];
	unsigned int inode;
} DirEnt_t;

typedef struct FsNode {
	char name[FILE_NAME_MAX];
	unsigned int flags; // node type, etc
	unsigned int inode;
	unsigned int start_sector;
	unsigned int length; // size in bytes.
	void* read;
	void* write;
	void* open;
	void* close;
	void* readdir;
	void* finddir;
	struct FsNode* ref; // Pointer to symlink or mount
} FsNode_t;


typedef int (*FsCallback)(struct File*);


int print(char* str);
int readline(char* buffer);
int exec(char* file, int argc, char** argv);
int sys_set_graphics_mode(int mode);
int dir_list(char* dir_name, char** buf);
int file_read(char* buf, int n, char* file_name);

#endif
