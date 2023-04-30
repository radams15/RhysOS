#include <stdio.h>
#include <string.h>
#include <syscall.h>

int entry(int argc, char** argv) { return main(argc, argv); }

void dir_listing(char* dir) {
	int i;
	char* dir_buf[20]; // allow for 20 files max.

	printf("Files in directory '%s'\n", dir);
	dir_list(dir, &dir_buf);
	
	for(i=0 ; i<5 ; i++) {		
		if(strlen(dir_buf[i]) == NULL)
			break;

		printf("\t- %s\n", dir_buf[i]);
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
