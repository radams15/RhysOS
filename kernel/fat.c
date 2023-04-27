#include "util.h"
#include "proc.h"

#define ROOT_DIR_SECTOR 2
#define SECTOR_SIZE 512
#define NAME_SIZE 10
#define HEADER_SIZE (NAME_SIZE+1) //Number of name and type bytes in directory
#define ENTRY_SIZE 32

extern int interrupt(int number, int AX, int BX, int CX, int DX);

void copyString(char* from, char* to, int length) {
  int i;
  for(i = 0; i < length; i++) {
    *(to + i) = *(from + i);
  }
}

void clearString(char* str, int length) {
  int i;
  for(i = 0; i < length; i++) {
    *(str + i) = 0;
  }
}

void moveString(char* from, char* to, int length) {
  copyString(from, to, length);
  clearString(from, length);
}


void parseFileName(char* name, char* top, char* sub) {
  int i = 0, x = 0, z;
  char *id;
  *top = '/';
  id = sub;

  if (*name == '/') {i++;}
  while((*(name + i) != '\0') && (*(name + i) != 0xA)) {
    if (*(name + i) == '/') {
      moveString(id, top, NAME_SIZE);
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

void loadRootDirectory(char* buffer) {read_sector(buffer, ROOT_DIR_SECTOR);}
void loadDirectory(char* buffer, char* name) {
  int sector;
  if (*name == '/') {
    loadRootDirectory(buffer);
  } else {
    /*sector = getDirSector(name);
    read_sector(buffer, sector);*/
  }
}

void directory(char* dirName) {
  int x, y;
  char directory[SECTOR_SIZE];

  loadDirectory(directory, dirName);

  for (x = 0; x < SECTOR_SIZE; x += ENTRY_SIZE) {
    if (directory[x] != 0) {
      for (y = 0; y < NAME_SIZE; y++) {
        print_char(directory[x + y]);
      }
      print_char('\n');
    }
  }
}

void readFile (char* filename, char outbuf[]) {
  int x, index;
  char entryChar;
  char directory[SECTOR_SIZE];
  char topName[NAME_SIZE], subName[NAME_SIZE];

  parseFileName(filename, topName, subName);
  loadDirectory(directory, topName);
  index = getDirIndex(subName, directory);
  
  if (index == -1) {
    print_string("ERROR: No such file in Directory\n"); 
    return;
  }

  for(x = HEADER_SIZE; x < ENTRY_SIZE; x++) {
    entryChar = directory[index + x];
    if (entryChar != 0) {
      read_sector((outbuf + (x - HEADER_SIZE) * SECTOR_SIZE), (int)entryChar);
    }
  }
}


void fat_test() {
	int ret;
	int size;
	char buf[1024];
	
    directory("/");
    
    readFile("/test.bin", &buf);
    
    ret = run_exe(&buf, sizeof(buf));
    
    print_string("\r\nReturn code: ");
    print_hex(ret);
    print_string("\n");
    
    return;
}
