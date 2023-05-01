#include "fs.h"

#include "util.h"
#include "proc.h"

extern int interrupt(int number, int AX, int BX, int CX, int DX);


void read_sector(int* buffer, int sector){	
	int relativeSector = mod(sector, 18) + 1;
	int track = sector / 36;
	int head = mod((sector / 18), 2);
	int floppyDevice = 0;

	interrupt(0x13, (2 * 256 + 1), (int)buffer, (track*256 + relativeSector), (head*256 + floppyDevice));
}

unsigned int fs_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	if(node->read != 0) {
		return node->read(node, offset, size, buffer);
	}
	
	return 0;
}

unsigned int fs_write(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	if(node->write != 0) {
		return node->write(node, offset, size, buffer);
	}
	
	return 0;
}

unsigned int fs_open(FsNode_t* node, unsigned char read, unsigned char write) {
	if(node->open != 0) {
		return node->open(node, read, write);
	}
	
	return 0;
}

unsigned int fs_close(FsNode_t* node) {
	if(node->close != 0) {
		return node->close(node);
	}
	
	return 0;
}

DirEnt_t* fs_readdir(FsNode_t* node, unsigned int index) {
	if(node->readdir != 0 && (node->flags&0x07) == FS_DIRECTORY) {
		return node->readdir(node, index);
	}
	
	return 0;
}

FsNode_t* fs_finddir(FsNode_t* node, char* name) {
	if(node->finddir != 0 && (node->flags&0x07) == FS_DIRECTORY) {
		return node->finddir(node, name);
	}
	
	return 0;
}

/*int fs_find_file(FsNode_t* root, char* name) {
	int i;
	int found;
	struct DirEnt* node = NULL;
	struct FsNode* fs_node;
	
	found = -1;
	
	while( (node = fs_readdir(root, i)) != NULL && found >= 0) {
		print_string(node->name);
		print_char('\n');
		
		fs_node = fs_finddir(root, node->name);
		
		if(fs_node == NULL) {
			print_string("FAIL\n");
		} else if ((fs_node->flags & 0x7) == FS_DIRECTORY) {
			found = fs_find_file(fs_node, name); // Recursively search directory
		} else {
			if(strcmp(node->name, name) == 0) {
				return i;
			}
		}
		
		i++;
	}
	
	return -1;
}*/
