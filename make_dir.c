#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int i;

	//open the floppy image
	FILE* floppy = fopen("build/kernel.bin","r+");
	if (floppy==0) {
		printf("floppya.img not found\n");
		return 1;
	}

	//load the disk map
	char map[512] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

	char dir[512] = "KERNEL\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC";

        fseek(floppy,512,SEEK_SET);
        for(i=0; i<512; i++)
		fputc(map[i],floppy);
        
        fseek(floppy,512*2,SEEK_SET);
        for (i=0; i<512; i++)
		fputc(dir[i],floppy);

	fclose(floppy);
	
	return 0;
}
