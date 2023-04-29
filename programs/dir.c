#include <stdio.h>
#include <string.h>
#include <syscall.h>

int entry(int argc, char** argv) { return main(argc, argv); }

void dir_listing(char* dir) {
	int i;
	File_t dir_buf[20]; // allow for 20 files max.
	File_t* file;

	printf("Files in directory '%s'\n", dir);
	dir_list(dir, &dir_buf);
	
	for(i=0 ; i<20 ; i++) {
		file = &dir_buf[i];
		
		if(strlen(file->name) == 0)
			break;

		printf("\t- %s\n", file->name);
	}
}

int main(int argc, char** argv) {
	int i;
	
	if(argc > 1) {
		for(i=1 ; i<argc ; i++) {
			dir_listing(argv[i]);
		}
	} else {
		dir_listing("/");
	}
	
	return 0;
}
