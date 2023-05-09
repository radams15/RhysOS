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
	unsigned int offset; // used by seek
	void* read;
	void* write;
	void* open;
	void* close;
	void* readdir;
	void* finddir;
	struct FsNode* ref; // Pointer to symlink or mount
} FsNode_t;


typedef int (*FsCallback)(struct File*);

int exec(char* file, int argc, char** argv);
int read(int fh, unsigned char* buffer, unsigned int size);
int write(int fh, unsigned char* buffer, unsigned int size);
int open(char* name);
void close(int fh);
void seek(int fh, unsigned int location);


void int2chars(unsigned int in, unsigned char* buffer);
int chars2int(unsigned char* buffer);

#endif
