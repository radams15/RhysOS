#include <stdio.h>

#define CHUNK_SIZE 0x64

int entry(int argc, char** argv) { return main(argc, argv); }

int type(char* file) {
	int n;
	char buffer[CHUNK_SIZE+1];
	int fh;
	int bytes_read = 1;
	int total_read = 0;
	int i;
	
	fh = open(file);
	
	if(fh == -1) {
		printf("Error reading file '%s'!\n", file);
		return 1;
	}
	
	do {
		seek(fh, total_read);
		
		bytes_read = read(fh, &buffer, CHUNK_SIZE);
		
		if(bytes_read == 0)
			break;
		
		write(stdout, buffer, bytes_read);
		total_read += bytes_read;
	} while(bytes_read > 0);
	
	close(fh);
	
	return 0;
}

int main(int argc, char** argv) {
	int i;
	
	for(i=1 ; i<argc ; i++) {
		type(argv[i]);
	}
	
	return 0;
}
