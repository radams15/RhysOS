#ifndef FAT_H
#define FAT_H

#include "fs.h"

FsNode_t* fat_init(int sector_start);

int fat_next_lba(int prev_lba);
int fat_next_cluster(int prev_cluster);

int cluster_to_lba(int cluster);
int lba_to_cluster(int lba);

FsNode_t* fat_create(char* name);

#endif
