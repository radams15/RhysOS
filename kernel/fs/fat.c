#include "fat.h"

#include "util.h"
#include "stdio.h"

#define MAX_FILES 64
#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct DirectoryEntry {
        union {
          struct {
	    char name[8];
	    char ext[3];
	  };
	  char fullname[11];
	};
	char attrib;
	char userattrib;
 
	char undelete;
	int createtime;
	int createdate;
	int accessdate;
	int clusterhigh;
 
	int modifiedtime;
	int modifieddate;
	int cluster;
	int filesize[2];
} __attribute__ ((packed));




unsigned int fat_table[512];
struct DirectoryEntry root_dir[MAX_FILES];

static FsNode_t root_node;
static FsNode_t root_nodes[MAX_FILES];
static int num_root_nodes;
static DirEnt_t dirent;

int cluster_to_lba(int cluster) {
  //  lba_addr = cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster; 
  // cluster_begin_lba = = (rsvd_secs + (num_fats * 32) + root_dir_sectors)
  int cluster_start = 33; //bpb.reserved_sector_count + (bpb.table_count * bpb.table_size_16) + bpb.root_entry_count; // TODO fix with BPB
  return cluster_start + (cluster-2) * 1;
}

unsigned int fat_read(FsNode_t* node, unsigned int byte_offset, unsigned int byte_size, unsigned char* out_buffer) {
    signed int cluster;
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

    cluster = cluster + (byte_offset / SECTOR_SIZE);
    sector_offset = byte_offset % SECTOR_SIZE;

    while (bytes_read < byte_size) {
        if(cluster >= 0xFF8)
          break;
    
        read_sector(&temp_buffer, cluster_to_lba(cluster));

        sector_bytes = MIN(SECTOR_SIZE - sector_offset, byte_size - bytes_read);

        memcpy(out_buffer + bytes_read, temp_buffer + sector_offset, sector_bytes);

        bytes_read += sector_bytes;
        cluster = fat_table[cluster];
        sector_offset = 0;
    }

    return bytes_read;
}

DirEnt_t* fat_readdir(FsNode_t* node, unsigned int index) {
	if(index >= num_root_nodes+1) {
		return NULL;
	}
	
	strcpy(dirent.name, root_nodes[index-1].name);
	dirent.name[strlen(root_nodes[index-1].name)] = 0;
	dirent.inode = root_nodes[index-1].inode;
	
	return &dirent;
}

FsNode_t* fat_finddir(FsNode_t* node, char* name) {
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

void fat_mount(FsNode_t* node, char* name) {
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

int list_sectors(struct DirectoryEntry* file) { // https://github.com/nathanhi/pyfatfs/blob/master/pyfatfs/PyFat.py#L810
  unsigned int cluster = file->cluster;
  
  while(1) {
    if(cluster >= 0xFF8)
      break;
    
    printi(cluster, 16);
    print_char(' ');
    
    cluster = fat_table[cluster];
  }
  
  print_string("\n");
}

void fat_load_root() {
  for(int i=0 ; i<MAX_FILES ; i++) {
    struct DirectoryEntry* entry = &root_dir[i];
    
    if(entry->name[0] != NULL) {
      print_string(entry->name); print_string("\n");
      
      strcpy(root_nodes[i].name, entry->fullname);
				
      root_nodes[i].flags = FS_FILE;
      root_nodes[i].inode = i;
      root_nodes[i].start_sector = entry->cluster;
      root_nodes[i].length = entry->filesize[0];
      root_nodes[i].offset = 0;
      root_nodes[i].read = fat_read;
      root_nodes[i].write = 0;
      root_nodes[i].open = 0;
      root_nodes[i].close = 0;
      root_nodes[i].readdir = 0;
      root_nodes[i].finddir = 0;
      root_nodes[i].ref = 0;
      num_root_nodes++;
    }
  }
}

FsNode_t* fat_init(int sector_start) {
  unsigned char fat_sector[512];
  
  sector_start = 1; // TODO: Fix
  
  for(int i=0 ; i<MAX_FILES ; i++) {
    root_dir[i].name[0] = NULL;
  }
  
  read_lba_to_segment(0, sector_start, &fat_sector, DATA_SEGMENT);
  read_lba_to_segment(0, sector_start+18, &root_dir, DATA_SEGMENT);
  //read_lba_to_segment(0, sector_start+19, &root_dir + 16, DATA_SEGMENT);
  
  unsigned char frame[3];
  int i, f1, f2, curr;
  curr = 0;
  for(i=0 ; i<512 ; i+=2) {
    frame[0] = fat_sector[curr];
    frame[1] = fat_sector[curr+1];
    frame[2] = fat_sector[curr+2];

    f1 = 0;
    f2 = 0;

    f1 |= frame[0];
    f1 &= 0x0FF;
    f1 |= (frame[1] & 0x0F)<<8;
    f2 = frame[2]<<4;
    f2 |= (frame[1] & 0xF0)>>4;
    f2 &= 0xFFF;

    fat_table[i] = f1;
    fat_table[i+1] = f2;

    curr += 3;
  }
  
  num_root_nodes = 0;
  
  strcpy(root_node.name, "/");
  root_node.flags = FS_DIRECTORY;
  root_node.inode = 0;
  root_node.length = 0;
  root_node.offset = 0;
  root_node.read = 0;
  root_node.write = 0;
  root_node.open = 0;
  root_node.close = 0;
  root_node.readdir = fat_readdir;
  root_node.finddir = fat_finddir;
  root_node.ref = 0;

  fat_load_root();
  
  FsNode_t* syscalls = fat_finddir(&root_node, "SYSCALLS.MD"); 
  
  return &root_node;
}
