#include <stdio.h>
#include <string.h>

struct BootEntry 
{ 
	unsigned char BS_jmpBoot[3] __attribute__((packed)); 
	unsigned char BS_OEMName[8] __attribute__((packed)); 
	unsigned short BPB_BytsPerSec __attribute__((packed)); 
	unsigned char BPB_SecPerClus __attribute__((packed)); 
	unsigned short BPB_RsvdSecCnt __attribute__((packed)); 
	unsigned char BPB_NumFATs __attribute__((packed)); 
	unsigned short BPB_RootEntCnt __attribute__((packed)); 
	unsigned short BPB_TotSec16 __attribute__((packed)); 
	unsigned char BPB_Media __attribute__((packed)); 
	unsigned short BPB_FATSz16 __attribute__((packed)); 
	unsigned short BPB_SecPerTrk __attribute__((packed)); 
	unsigned short BPB_NumHeads __attribute__((packed)); 
	unsigned long BPB_HiddSec __attribute__((packed)); 
	unsigned long BPB_TotSec32 __attribute__((packed)); 
	unsigned long BPB_FATSz32 __attribute__((packed));
	unsigned short BPB_ExtFlags __attribute__((packed));
	unsigned short BPB_FSVer __attribute__((packed));
	unsigned long BPB_RootClus __attribute__((packed));
	unsigned short BPB_FSInfo __attribute__((packed));
	unsigned short BPB_BkBootSec __attribute__((packed));
	unsigned char BPB_Reserved[12] __attribute__((packed));
	unsigned char BS_DrvNum __attribute__((packed));
	unsigned char BS_Reserved1 __attribute__((packed));
	unsigned char BS_BootSig __attribute__((packed));
	unsigned long BS_VolID __attribute__((packed));
	unsigned char BS_VolLab[11] __attribute__((packed));
	unsigned char BS_FilSysType[8] __attribute__((packed));
};

int main(){
	FILE* f = fopen("template.img", "r");

    int data[512];
    fread(data, sizeof(char), 512, f);
    
    struct BootEntry* entry = (struct BootEntry*) data;
    
    printf("%d\n", entry->BPB_BytsPerSec);

	fclose(f);

	return 0;
}
