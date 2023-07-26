#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define FAT_ENTRIES 512

const char* fname = "../build/rootfs.img";
int fh;

int* decodeFAT2(int fp){
   int * fat = (int*)malloc(FAT_ENTRIES * sizeof(int));
   if(fat == NULL) exit(-1);

   char frame[3]; // read 3 bytes at a time and decode
   int i, f1, f2;
   for(i = 0; i < FAT_ENTRIES; i+=2){
      f1 = 0;
      f2 = 0;
      read(fp, &frame, 3);
      f1 |= frame[0];
      f1 &= 0x0FF;
      f1 |= (frame[1] & 0x0F)<<8;
      f2 = frame[2]<<4;
      f2 |= (frame[1] & 0xF0)>>4;
      f2 &= 0xFFF;
      fat[i] = f1;
      fat[i+1] = f2;
   }

   return fat;
}


int* decodeFAT(int fp){
    uint8_t raw[512];
    int * fat = (int*)malloc(FAT_ENTRIES * sizeof(int));
    if(fat == NULL) exit(-1);

    read(fh, &raw, 512);

    uint8_t frame[3];
    int i, f1, f2;
    int curr = 0;
    for(i = 0; i < FAT_ENTRIES; i+=2){
        frame[0] = raw[curr];
        frame[1] = raw[curr+1];
        frame[2] = raw[curr+2];

        f1 = 0;
        f2 = 0;

        f1 |= frame[0];
        f1 &= 0x0FF;
        f1 |= (frame[1] & 0x0F)<<8;
        f2 = frame[2]<<4;
        f2 |= (frame[1] & 0xF0)>>4;
        f2 &= 0xFFF;

        fat[i] = f1;
        fat[i+1] = f2;
        curr += 3;
    }

   return fat;
}

void fat_test(int sector_start) {
    lseek(fh, 512*1, SEEK_SET);
    int* fat = decodeFAT(fh);

    for(int i=0 ; i < 64 ; i++) {
        printf("%03x ", fat[i]);
    }

    free(fat);
}

int main() {
    fh = open(fname, O_RDONLY);

    if(fh == -1)
        return 1;

    fat_test(1);

    close(fh);

    return 0;
}
