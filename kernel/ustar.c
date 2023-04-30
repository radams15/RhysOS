#include "ustar.h"

#define ROOT_DIR_SECTOR 1
#define SECTOR_SIZE 512
#define NAME_SIZE 10
#define HEADER_SIZE (NAME_SIZE+1) //Number of name and type bytes in directory
#define ENTRY_SIZE 32

void moveString(char* to, char* from, int length) {
  strcpy(to, from, length);
  clear(from, length);
}

void read_sector(int* buffer, int sector){	
	int relativeSector = mod(sector, 18) + 1;
	int track = sector / 36;
	int head = mod((sector / 18), 2);
	int floppyDevice = 0;

	interrupt(0x13, (2 * 256 + 1), (int)buffer, (track*256 + relativeSector), (head*256 + floppyDevice));
}

void file_struct_copy(struct File* out, union tar_t* in, int sector, int size) {
	memcpy(out->name, in->old.name, strlen(in->old.name));
	out->sector_start = sector+1; // +1 to remove header sector
	out->size = size;
}

int ustar_list_directory(char* dir_name, struct File* buf) {
	int sector;
	int size_sectors;
	int size;
	int i;
	union tar_t file;
	struct File out_file;
	
	if(dir_name[0] == '/' && dir_name[1] == 0) {
		sector = ROOT_DIR_SECTOR;
	} else {
		print_string("Unknown Directory");
		return;
	}
	
	read_sector(&file, sector);
	
	i=0;
	while(strcmp("ustar", file.new.ustar) == 0) {
		size = oct2bin(file.old.size, 11);
		
		size_sectors = (size / SECTOR_SIZE) + 2;
		
		if(buf != NULL) {
			file_struct_copy(&out_file, &file, sector, size);
			
			memcpy(&buf[i], &out_file, sizeof(struct File));
			i++;
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
	
	sector = ROOT_DIR_SECTOR;
	
	read_sector(&file, sector);
	
	while(strcmp("ustar", file.new.ustar) == 0) {
		size = oct2bin(file.old.size, 11);
		
		size_sectors = (size / SECTOR_SIZE) + 2;
		
		if(strcmp(file.old.name, filename) == 0) { // Correct File
			goto found;
		}
		
		sector += size_sectors;
		
		read_sector(&file, sector);
	}
	
	return 1;
	
found:
	file_struct_copy(out, &file, sector, size);
  
	return 0;
}

int ustar_read_file(char* buf, int n, char* filename) {
	int sector;
	int read;
	int size_sectors;
	int end_sector;
	int to_copy;
	
	char sect_buf[SECTOR_SIZE];
	
	struct File file;
	
	if(find_file(&file, filename)) {
		print_string("Could not find file\n");
		return 1;
	}
	
	size_sectors = (file.size/SECTOR_SIZE);
	end_sector = file.sector_start + size_sectors;
	read = 0;
	
	for(sector=file.sector_start ; sector <= end_sector ; sector++) {	
		read_sector(&sect_buf, sector);
		
		if(read + SECTOR_SIZE > n) {
			to_copy = n-read;
		} else {
			to_copy = SECTOR_SIZE;
		}
		
		memcpy(buf, &sect_buf, to_copy);
		
		buf += to_copy;
		read += SECTOR_SIZE;
	}
	
	return 0;
}
