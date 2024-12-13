#ifndef SYSCALL_H
#define SYSCALL_H

#define FILE_NAME_MAX 32

extern int interrupt(int number, int AX, int BX, int CX, int DX);

typedef struct DirEnt {
    char name[FILE_NAME_MAX];
    unsigned int inode;
} DirEnt_t;

typedef struct FsNode {
    char name[FILE_NAME_MAX];
    unsigned int flags;  // node type, etc
    unsigned int inode;
    unsigned int start_sector;
    unsigned int length;  // size in bytes.
    unsigned int offset;  // used by seek
    void* read;
    void* write;
    void* open;
    void* close;
    void* readdir;
    void* finddir;
    struct FsNode* ref;  // Pointer to symlink or mount
} FsNode_t;

typedef struct Stat {
    char name[FILE_NAME_MAX];
    unsigned int flags;
    unsigned int inode;
    unsigned int length;
    unsigned int offset;
} Stat_t;

typedef enum FileMode {
    O_CREAT = 0b00000001,
    O_APPEND = 0b00000010,
    O_RDONLY = 0b00000100,
    O_WRONLY = 0b00001000,
} FileMode_t;

typedef int (*FsCallback)(struct FsNode*);

int execa(char* file, int argc, char** argv, int in, int out, int err);
int read(int fh, unsigned char* buffer, unsigned int size);
int write(int fh, unsigned char* buffer, unsigned int size);
int open(char* name, FileMode_t mode);
int close(int fh);
int seek(int fh, unsigned int location);
int dir_list(char* dir_name, struct FsNode* buf, int max);
int kfree(void* ptr);
int stat(const char* name, Stat_t* stat);

int interrupt_10(int AX, int BX, int CX, int DX);
int interrupt_21(int AX, int BX, int CX, int DX);

#define exec(file, argc, argv) execa(file, argc, argv, stdin, stdout, stderr)

int int2chars(unsigned int in, unsigned char* buffer);
int chars2int(unsigned char* buffer);

#endif
