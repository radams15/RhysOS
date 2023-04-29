#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fs_structs.h"

int main(int argc, char* argv[]) {
	int i;

	//open the floppy image
	FILE* floppy = fopen("build/kernel.bin","r+");
	if (floppy==0) {
		printf("floppya.img not found\n");
		return 1;
	}

	//load the disk map
	char map[512] = {0};
	
	for(i=0 ; i<KERNEL_SECTORS ; i++)
		map[i] = 0xFF;

	struct DirEnt dir[512] = {
		{
			"KERNEL",
			"IMG",
			0,
		}
	};

	fseek(floppy,512,SEEK_SET);
	for(i=0; i<512; i++)
		fputc(map[i],floppy);

	fseek(floppy,512*2,SEEK_SET);
	for (i=0; i<512; i++)
		fputc(((char*)dir)[i],floppy);


	fclose(floppy);
	
	return 0;
}
