int main();
void entry() { main(); }

#define CODE_SEGMENT 0x100
#define DATA_SEGMENT 0x900

#define SECTORS_PER_TRACK 18
#define BYTES_PER_SECTOR 512

#define KERNEL_SECT 6

int interrupt (int number, int AX, int BX, int CX, int DX);

void read_sector(int disk, int track, int head, int sector, int dst_addr, int dst_seg);

void call_kernel();

void printi(int num, int base);

void read_sector_lba(int disk, int lba, int dst_addr, int dst_seg) {
	int head = (lba % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK;
	int track = (lba / (SECTORS_PER_TRACK * 2));
	int sector = (lba % SECTORS_PER_TRACK + 1);

    read_sector(disk, track, head, sector, dst_addr, dst_seg);
}

void read_sector_lba_dummy(int disk, int lba, int dst_addr, int dst_seg) {
  print("Read sector ");
  printi(lba, 16);
  print(" to: ");
  printi(dst_seg, 16);
  print(":");
  printi(dst_addr, 16);
  print("\r\n");
  
  read_sector_lba(disk, lba, dst_addr, dst_seg);
}

void printc(char c) {
    interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void print(char* str) {
    char* c;
    for(c = str ; *c != 0 ; c++)
        printc(*c);
}

void printi(int num, int base) {
    char buffer[64];
    char* ptr = &buffer[sizeof(buffer)-1];
    int remainder;
    
    if(base == 0) {
    	print("Cannot have a base of 0!\r\n");
    	return;
    }
    
    *ptr = '\0';

    if (num == 0) {
        printc('0');
        return;
    }

    while (num != 0) {
        remainder = imod(num, base);
        
        if (remainder < 10)
            *--ptr = '0' + remainder;
        else
            *--ptr = 'A' + remainder - 10;
        
        num /= base;
    }

    while (*ptr != '\0') {
        printc(*ptr++);
    }
}

struct Header {
    char magic[2];
    short textstart;
    short datastart;
    short textsize;
    short datasize;
    char other[512];
};

extern char getbit();

int main() {
    struct Header header;
    
    read_sector_lba(0, KERNEL_SECT, &header, 0x0050);

    print("Magic: '");
    print(header.magic);
    print("'\r\n");
    if(header.magic[0] != 'R' && header.magic[1] != 'Z') {
      print("Invalid kernel magic, abort.\r\n");
      goto main_end;
    }

    // Load the text segment.

    int sector;
    int i;
    int addr = 0x50; // Read to address 0 in CODE_SEGMENT
    for(i=0 ; i<header.textsize ; i++) {
        sector = header.textstart + i;

        read_sector_lba_dummy(0, sector, addr, CODE_SEGMENT);
        addr += BYTES_PER_SECTOR;
    }

    addr = 0x5000; // Read to address 0 in DATA_SEGMENT
    for(i=0 ; i<header.datasize ; i++) {
        sector = header.datastart + i;

        read_sector_lba_dummy(0, sector, addr, DATA_SEGMENT);
        addr += BYTES_PER_SECTOR;
    }

    print("Loaded!\r\n");

    call_kernel();

    print("Done!\r\n");

main_end:
    for(;;) {}
}
