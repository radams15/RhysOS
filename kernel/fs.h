#ifndef RHYSOS_FS_H
#define RHYSOS_FS_H

#include "type.h"

typedef struct File {
	char name[100];
	unsigned int sector_start;
	unsigned int size; // size in bytes.
} File_t;

#endif
