#ifndef FS_STRUCTS_H
#define FS_STRUCTS_H

typedef char DiskMap[512];

struct DirEnt {
	char file_name[7];
	char ext[3];
	int sector;
};

typedef struct DirEnt DirectoryTable[16]; // 512/32 = 16

#endif
