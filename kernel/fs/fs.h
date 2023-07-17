#ifndef RHYSOS_FS_H
#define RHYSOS_FS_H

#include "type.h"

#define FILE_NAME_MAX 128

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

/*
	http://www.jamesmolloy.co.uk/tutorial_html/8.-The%20VFS%20and%20the%20initrd.html
*/

void read_sector(int* buffer, int sector);
void read_sector_to_segment(int disk, int track, int head, int sector, int dst_addr, int dst_seg);
void read_lba_to_segment(int disk, int lba, int dst_addr, int dst_seg);

typedef unsigned int (*ReadFunc)(struct FsNode*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*WriteFunc)(struct FsNode*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*OpenFunc)(struct FsNode*, unsigned char, unsigned char);
typedef unsigned int (*CloseFunc)(struct FsNode*);
typedef struct DirEnt* (*ReaddirFunc)(struct FsNode*, unsigned int);
typedef struct FsNode* (*FinddirFunc)(struct FsNode*, char* name);

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
	ReadFunc read;
	WriteFunc write;
	OpenFunc open;
	CloseFunc close;
	ReaddirFunc readdir;
	FinddirFunc finddir;
	struct FsNode* ref; // Pointer to symlink or mount
} FsNode_t;


unsigned int fs_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int fs_write(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int fs_open(FsNode_t* node, unsigned char read, unsigned char write);
unsigned int fs_close(FsNode_t* node);
DirEnt_t* fs_readdir(FsNode_t* node, unsigned int index);
FsNode_t* fs_finddir(FsNode_t* node, char* name);

FsNode_t* get_dir(char* name);
int open(char* name);
void close(int fh);
void seek(int fh, unsigned int location);

extern FsNode_t* fs_root;

#endif
