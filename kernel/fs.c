#include "fs.h"

#include "util.h"
#include "proc.h"

extern int interrupt(int number, int AX, int BX, int CX, int DX);

unsigned int fs_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int fs_write(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer);
void fs_open(FsNode_t* node, unsigned char read, unsigned char write);
void fs_close(FsNode_t* node);
DirEnt_t* fs_readdir(FsNode_t* node, unsigned int index);
FsNode_t* fs_finddir(FsNode_t* node, char* name); 
