#ifndef FAT_H
#define FAT_H

#include "fs.h"

FsNode_t* fat_init(int sector_start);
void fat_mount(FsNode_t* node, char* name);

#endif
