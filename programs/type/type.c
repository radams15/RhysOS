#include <stdio.h>

#define CHUNK_SIZE 512

int entry(int argc, char** argv) { return main(argc, argv); }

int type(char* file) {
	int n;
	char buffer[CHUNK_SIZE+1];
	int fh;
	
	fh = open(file);
	
	if(fh == -1) {
		printf("Error!\n");
		return 1;
	}
	
	print_hex_2(fh);
	
	read(fh, &buffer, 10);
	
	close(fh);
	
	printf("%s\n", buffer);
	
	return 0;
}

int main(int argc, char** argv) {
	int i;
	
	for(i=1 ; i<argc ; i++) {
		type(argv[i]);
	}
	
	return 0;
}
