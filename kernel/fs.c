#include "fs.h"

#include "util.h"
#include "proc.h"

#define ROOT_DIR_SECTOR 1
#define SECTOR_SIZE 512
#define NAME_SIZE 10
#define HEADER_SIZE (NAME_SIZE+1) //Number of name and type bytes in directory
#define ENTRY_SIZE 32

extern int interrupt(int number, int AX, int BX, int CX, int DX);

void strcpy(char* to, char* from, int length) {
  int i;
  for(i = 0; i < length; i++) {
    *(to + i) = *(from + i);
  }
}

void moveString(char* to, char* from, int length) {
  strcpy(to, from, length);
  clear(from, length);
}


void parseFileName(char* name, char* top, char* sub) {
  int i = 0, x = 0, z;
  char *id;
  *top = '/';
  id = sub;

  if (*name == '/') {i++;}
  
  while((*(name + i) != '\0') && (*(name + i) != 0xA)) {
    if (*(name + i) == '/') {
      moveString(top, id, NAME_SIZE);
      for(z = x; z < NAME_SIZE && z > 0; z++) {*(top + z) = 0;}
      x = 0;
    } else {
      *(id + x) = *(name + i);
      x++; 
    }
    i++;
  }
  for(z = x; z < NAME_SIZE; z++) {*(sub + z) = 0;}
}


void read_sector(int* buffer, int sector){	
	int relativeSector = mod(sector, 18) + 1;
	int track = sector / 36;
	int head = mod((sector / 18), 2);
	int floppyDevice = 0;

	interrupt(0x13, (2 * 256 + 1), (int)buffer, (track*256 + relativeSector), (head*256 + floppyDevice));
}


int getDirIndex(char* filename, char* directory) {
  int x, y, index, matches;
  char fileChar, entryChar;

  for (x = 0; x < SECTOR_SIZE; x += ENTRY_SIZE) {
    matches = 1;
    index = -1;

    for (y = 0; y < NAME_SIZE; y++) {
      fileChar = *(filename + y);
      entryChar = *(directory + x + y);

      if ((fileChar == 0xA) && (entryChar == 0)) {
        break;
      } else if (fileChar != entryChar) {
        matches = 0;
        break;
      }
    }

    if (matches) {
      index = x;
      break;
    } 
  }

  return index;
}

int list_directory(char* dir_name, FsCallback callback, void* data) {
	int sector;
	int size_sectors;
	int size;
	union tar_t file;
	
	if(dir_name[0] == '/' && dir_name[1] == 0) {
		sector = ROOT_DIR_SECTOR;
	} else {
		print_string("Unknown Directory");
		return;
	}
	
	read_sector(&file, sector);
	
	while(strcmp("ustar", file.new.ustar) == 0) {
		size = oct2bin(file.old.size, 11);
		
		size_sectors = (size / SECTOR_SIZE) + 2;
		
		if(callback != NULL) {
			if(! callback(file, data)) {
				break;
			}
		} else {
			print_string(file.old.name);
			print_char('\n');
		}
		
		sector += size_sectors;
		
		read_sector(&file, sector);
	}
	
	return 0;
}

int find_file(struct File* out, char* filename) {
	int x, index;
	char entryChar;
	char directory[SECTOR_SIZE];
	char topName[NAME_SIZE], subName[NAME_SIZE];
	int sector;
	int size_sectors;
	int size;
	
	union tar_t file;

	parseFileName(filename, topName, subName);
	
	/*if(topName[0] == '/' && topName[1] == 0) {
		sector = ROOT_DIR_SECTOR;
	} else {
		print_string("Unknown Directory");
		return;
	}*/
	sector = ROOT_DIR_SECTOR;
	
	read_sector(&file, sector);
	
	while(strcmp("ustar", file.new.ustar) == 0) {
		size = oct2bin(file.old.size, 11);
		
		size_sectors = (size / SECTOR_SIZE) + 2;
		
		if(strcmp(file.old.name, subName) == 0) { // Correct File
			goto found;
		}
		
		sector += size_sectors;
		
		read_sector(&file, sector);
	}
	
	return 1;
	
found:

	//memcpy(out->name, file.old.name, sizeof(out->name));
	out->sector_start = sector+1; // +1 to remove header sector
	out->size = size;
  
	return 0;
}

int read_file(char* buf, char* filename) {
	int sector;
	int size_sectors;
	int end_sector;
	
	struct File file;
	
	if(find_file(&file, filename)) {
		print_string("Could not find file\n");
		return 1;
	}
	
	size_sectors = (file.size/512);
	end_sector = file.sector_start + size_sectors;
	
	for(sector=file.sector_start ; sector <= end_sector ; sector++) {
		read_sector(buf, sector);
		buf += 512;
	}
	
	return 0;
}
