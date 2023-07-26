#include "fat.h"

#include "stdio.h"

unsigned int fat_table[512];

struct DirectoryEntry {
	char name[8];
	char ext[3];
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

struct DirectoryEntry root_dir[32];

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

void fat_init(int sector_start) {
  unsigned char fat_sector[512];
  int disk = 1;
  
  sector_start = 1; // TODO: Fix
  
  read_lba_to_segment(disk, sector_start, &fat_sector, DATA_SEGMENT);
  read_lba_to_segment(disk, sector_start+18, &root_dir, DATA_SEGMENT);
  
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
  
  for(int i=0 ; i<32 ; i++) {
      printi(fat_table[i], 16);
    
      print_char(' ');
  }
  
  print_string("\n-----\n");
  
  print_string(root_dir[0].name);
  print_string(" => ");

  //list_sectors(&root_dir[0]);
}
