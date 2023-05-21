#include "ustar.h"
#include "malloc.h"
#include "util.h"

#define ROOT_DIR_SECTOR 1
#define SECTOR_SIZE 512
#define NAME_SIZE 10
#define HEADER_SIZE (NAME_SIZE+1) //Number of name and type bytes in directory
#define ENTRY_SIZE 32

#define MAX_FILES 32

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int sector_start;
static FsNode_t* root_node;
static FsNode_t* root_nodes;
static unsigned int num_root_nodes;

static DirEnt_t dirent;


unsigned int ustar_read(node, byte_offset, byte_size, out_buffer)
	FsNode_t* node;
	unsigned int byte_offset;
	unsigned int byte_size;
	unsigned char* out_buffer;
{
    signed int start_sector;
    unsigned int sector_offset;
    unsigned int sector_bytes;
    unsigned int bytes_read = 0;
    unsigned int end_byte_offset;
    unsigned char temp_buffer[SECTOR_SIZE];
    int i;

    end_byte_offset = byte_offset + byte_size;

    if (end_byte_offset > node->length) {
        byte_size = node->length - byte_offset;
    }

    if (byte_size <= 0) {
        return 0;
    }

    start_sector = node->start_sector + (byte_offset / SECTOR_SIZE);
    sector_offset = byte_offset % SECTOR_SIZE;

    while (bytes_read < byte_size) {
        read_sector((int*) &temp_buffer, start_sector);

        sector_bytes = MIN(SECTOR_SIZE - sector_offset, byte_size - bytes_read);

        memcpy(out_buffer + bytes_read, temp_buffer + sector_offset, sector_bytes);

        bytes_read += sector_bytes;
        start_sector++;
        sector_offset = 0;
    }
    
    out_buffer[bytes_read] = 0; // Null terminate, just in case it's a string.

    return bytes_read;
}

unsigned int ustar_write(FsNode_t* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
	return 0;
}

DirEnt_t* ustar_readdir(FsNode_t* node, unsigned int index) {
	if(index >= num_root_nodes+1) {
		return NULL;
	}
	
	strcpy(dirent.name, root_nodes[index-1].name);
	dirent.name[strlen(root_nodes[index-1].name)] = 0;
	dirent.inode = root_nodes[index-1].inode;
	
	return &dirent;
}

FsNode_t* ustar_finddir(FsNode_t* node, char* name) {
   int i;
   for (i = 0; i < num_root_nodes; i++) {
       if (strcmp(name, root_nodes[i].name) == 0) {
       		if((root_nodes[i].flags & 0x08)) { // Is a mount
       			return root_nodes[i].ref;
   			} else {
	           return &root_nodes[i];
           }
       }
   }
   
   return NULL;
}

int ustar_load_root() {
	int sector;
	int size_sectors;
	int size;
	int i;
	union Tar file;
	
	sector = ROOT_DIR_SECTOR;
	
	read_sector((int*) &file, sector);
	
	i=0;
	while(strcmp("ustar", file.new.ustar) == 0) {
		size = oct2bin(file.old.size, 11);
		
		size_sectors = (size / SECTOR_SIZE) + 2;
		
		strcpy(root_nodes[i].name, file.old.name);
		
		root_nodes[i].flags = FS_FILE;
		root_nodes[i].inode = i;
		root_nodes[i].start_sector = sector+1; // +1 to ignore header
		root_nodes[i].length = size;
		root_nodes[i].offset = 0;
		root_nodes[i].read = ustar_read;
		root_nodes[i].write = 0;
		root_nodes[i].open = 0;
		root_nodes[i].close = 0;
		root_nodes[i].readdir = 0;
		root_nodes[i].finddir = 0;
		root_nodes[i].ref = 0;
		
		i++;
		sector += size_sectors;
		
		read_sector((int*) &file, sector);
	}
	
	num_root_nodes = i;
	
	return 0;
}


void ustar_mount(FsNode_t* node, char* name) {
	int i = num_root_nodes;
	
	strcpy(root_nodes[i].name, name);
	root_nodes[i].flags = FS_DIRECTORY | FS_MOUNTPOINT;
	root_nodes[i].inode = i;
	root_nodes[i].length = 1;
	root_nodes[i].offset = 0;
	root_nodes[i].read = 0;
	root_nodes[i].write = 0;
	root_nodes[i].open = 0;
	root_nodes[i].close = 0;
	root_nodes[i].readdir = 0;
	root_nodes[i].finddir = 0;
	root_nodes[i].ref = node;
	
	num_root_nodes++;
}

FsNode_t* ustar_init(int fs_sector_start) {
	int i;
	
	sector_start = fs_sector_start;
	
	root_node = malloc(sizeof(FsNode_t));
	
	strcpy(root_node->name, "/");
	root_node->flags = FS_DIRECTORY;
	root_node->inode = 0;
	root_node->length = 0;
	root_nodes[i].offset = 0;
	root_node->read = 0;
	root_node->write = 0;
	root_node->open = 0;
	root_node->close = 0;
	root_node->readdir = ustar_readdir;
	root_node->finddir = ustar_finddir;
	root_node->ref = 0;
	
	
	root_nodes = malloc(sizeof(FsNode_t) * MAX_FILES);
	
	ustar_load_root();
	
	return root_node;
}
