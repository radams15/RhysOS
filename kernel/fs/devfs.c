#include "devfs.h"

#include "util.h"

#define MAX_FILES 8

static FsNode_t* root_node;
static FsNode_t* root_nodes;
static int num_root_nodes;

static DirEnt_t dirent;

DirEnt_t* devfs_readdir(FsNode_t* node, unsigned int index) {
	if(index >= num_root_nodes+1) {
		return NULL;
	}
	
	strcpy(dirent.name, root_nodes[index-1].name);
	dirent.name[strlen(root_nodes[index-1].name)] = 0;
	dirent.inode = root_nodes[index-1].inode;
	
	return &dirent;
}

FsNode_t* devfs_finddir(FsNode_t* node, char* name) {
   int i;
   for (i = 0; i < num_root_nodes; i++) {
       if (strcmp(name, root_nodes[i].name) == 0) {
           return &root_nodes[i];
       }
   }
   
   return NULL;
}

void stdout_write(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	int i;
	
	for(i=offset ; i<size ; i++) {
		print_char(buffer[i]);
	}
}

void stderr_write(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	int i;
	
	for(i=offset ; i<size ; i++) {
		print_char(buffer[i]);
	}
}

void stdin_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	int i;
	
	for(i=0 ; i<size ; i++) {
		buffer[i] = getch();
	}
}

void int2chars(unsigned int in, unsigned char* buffer) {
	buffer[0] = (unsigned char) in & 0xff; // low byte
	buffer[1] = (in >> 8) & 0xff; // high byte
}

void lowmem_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	unsigned int mem = lowmem();
	int2chars(mem, buffer);
}


void highmem_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	int mem = highmem();
	int2chars(mem, buffer);
}

void mem_read(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	int mem = lowmem() + highmem();
	int2chars(mem, buffer);
}

void devfs_setup() {
	int i = 0;
	
	strcpy(root_nodes[i].name, "stdout");
	root_nodes[i].flags = FS_FILE;
	root_nodes[i].inode = i;
	root_nodes[i].length = 1;
	root_nodes[i].offset = 0;
	root_nodes[i].read = 0;
	root_nodes[i].write = stdout_write;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = 0;
	num_root_nodes++;
	
	i++;
	
	strcpy(root_nodes[i].name, "stdin");
	root_nodes[i].flags = FS_FILE;
	root_nodes[i].inode = i;
	root_nodes[i].length = 1;
	root_nodes[i].offset = 0;
	root_nodes[i].read = stdin_read;
	root_nodes[i].write = 0;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = 0;
	num_root_nodes++;
	
	i++;
	
	strcpy(root_nodes[i].name, "stderr");
	root_nodes[i].flags = FS_FILE;
	root_nodes[i].inode = i;
	root_nodes[i].length = 1;
	root_nodes[i].offset = 0;
	root_nodes[i].read = 0;
	root_nodes[i].write = stderr_write;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = 0;
	num_root_nodes++;
	
	i++;
	
	strcpy(root_nodes[i].name, "highmem");
	root_nodes[i].flags = FS_FILE;
	root_nodes[i].inode = i;
	root_nodes[i].length = 2;
	root_nodes[i].offset = 0;
	root_nodes[i].read = highmem_read;
	root_nodes[i].write = 0;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = 0;
	num_root_nodes++;
	
	i++;
	
	strcpy(root_nodes[i].name, "lowmem");
	root_nodes[i].flags = FS_FILE;
	root_nodes[i].inode = i;
	root_nodes[i].length = 2;
	root_nodes[i].offset = 0;
	root_nodes[i].read = lowmem_read;
	root_nodes[i].write = 0;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = 0;
	num_root_nodes++;
	
	i++;
	
	strcpy(root_nodes[i].name, "mem");
	root_nodes[i].flags = FS_FILE;
	root_nodes[i].inode = i;
	root_nodes[i].length = 2;
	root_nodes[i].offset = 0;
	root_nodes[i].read = mem_read;
	root_nodes[i].write = 0;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = 0;
	num_root_nodes++;
}


FsNode_t* devfs_init() {
	root_node = malloc(sizeof(FsNode_t));
	
	strcpy(root_node->name, "dev");
	root_node->flags = FS_DIRECTORY;
	root_node->inode = 0;
	root_node->length = 0;
	root_node->offset = 0;
	root_node->read = 0;
	root_node->write = 0;
	root_node->open = 0;
	root_node->close = 0;
	root_node->readdir = devfs_readdir;
	root_node->finddir = devfs_finddir;
	root_node->ref = 0;
	
	num_root_nodes = 0;
	root_nodes = malloc(sizeof(FsNode_t) * MAX_FILES);
	
	if(root_node >= root_nodes) {
		print_string("FAILLLLL\n");
	}
	
	devfs_setup();
	
	return root_node;
}
