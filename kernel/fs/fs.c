#include "fs.h"

#include "util.h"
#include "proc.h"

#define MAX_OPEN_FILES 32

extern int interrupt(int number, int AX, int BX, int CX, int DX);

FsNode_t* open_files[MAX_OPEN_FILES];
FsNode_t* fs_root;

FsNode_t* fh_get_node(int fh) {
	return open_files[fh];
}

int write(int fh, unsigned char* buffer, unsigned int size) {
	return fs_write(fh_get_node(fh), fh_get_node(fh)->offset, size, buffer);
}

int read(int fh, unsigned char* buffer, unsigned int size) {
	return fs_read(fh_get_node(fh), fh_get_node(fh)->offset, size, buffer);
}

void seek(int fh, unsigned int location) {
	fh_get_node(fh)->offset = location;
}

int open(char* name) {
	int i;
	FsNode_t* handle;
	
	handle = get_dir(name);
	
	if(handle == NULL) {
		return -1;
	}
	
	for(i=0 ; i<MAX_OPEN_FILES ; i++) {
		if(open_files[i] == NULL) {
			open_files[i] = handle;
			return i;
		}
	}

	return -1;
}

void close(int fh) {
	open_files[fh] = NULL;
}

FsNode_t* get_dir(char* name) {
	FsNode_t* fsnode = fs_root;
	char* tok;
	
	char buf[256];
	memcpy(&buf, name, 100);
	buf[strlen(name)] = 0;
	
	tok = strtok(buf, "/");
	
	while(tok != NULL && fsnode != NULL) {
		fsnode = fs_finddir(fsnode, tok);
		
		tok = strtok(NULL, "/");
	}
	
	return fsnode;
}

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
