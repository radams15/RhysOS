int main();
void entry() { main(); }

#define CODE_SEGMENT 0x2000
#define DATA_SEGMENT 0x3000

#define SECTORS_PER_TRACK 18
#define BYTES_PER_SECTOR 512

#define FS_SECT 0

int interrupt (int number, int AX, int BX, int CX, int DX);

void read_sector(int disk, int track, int head, int sector, int dst_addr, int dst_seg);

void call_kernel();

void printi(int num, int base);

typedef union Tar {
        // Pre-POSIX.1-1988 format
        struct {
            char name[100];             // file name
            char mode[8];               // permissions
            char uid[8];                // user id (octal)
            char gid[8];                // group id (octal)
            char size[12];              // size (octal)
            char mtime[12];             // modification time (octal)
            char check[8];              // sum of unsigned characters in block, with spaces in the check field while calculation is done (octal)
            char link;                  // link indicator
            char link_name[100];        // name of linked file
        } old;

        // UStar format (POSIX IEEE P1003.1)
        struct {
            char old[156];              // first 156 octets of Pre-POSIX.1-1988 format
            char type;                  // file type
            char also_link_name[100];   // name of linked file
            char ustar[8];              // ustar\000
            char owner[32];             // user name (string)
            char group[32];             // group name (string)
            char major[8];              // device major number
            char minor[8];              // device minor number
            char prefix[155];
            struct tar_t * next;
        } new;
        
        char buffer[512];
} Tar_t;

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
    if(c == '\n')
      printc('\r');
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
    	print("Cannot have a base of 0!\n");
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

int oct2bin(unsigned char* str, int size) {
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

int ceil_division(int dividend, int divisor) {
    int quotient = dividend / divisor;    // Perform the division

    if (dividend % divisor != 0) {
        quotient++;   // Increment the quotient if there is a remainder
    }

    return quotient;
}

int load_segment(int disk, int sect_start, int sect_count, int dst_addr, int dst_seg) {
  int sect;
  int addr = dst_addr;
  for(int i=0 ; i<sect_count ; i++) {
    sect = sect_start + i;
    
    read_sector_lba_dummy(disk, sect, dst_addr, dst_seg);
    
    dst_addr += BYTES_PER_SECTOR;
  }
}

int strncmp(char* a, char* b, int len) {
  for(int i=0 ; i<len ; i++) {
    if(a[i] != b[i])
      return 1;
  }
  
  return 0;
}

int main() {
    Tar_t header;
    int sect = FS_SECT;
    
    while(1) {
      sect++;
      read_sector_lba(0, sect, &header, 0x50);
      
      if(strncmp(header.new.ustar, "ustar", 5) == 0) {
        int length = oct2bin(header.old.size, 11);
        int size_sectors = ceil_division(length, BYTES_PER_SECTOR);
        
        if(strncmp(header.old.name, "kernel.text", 11) == 0) {
          print(header.old.name);
          print(" => Length: ");
          printi(size_sectors, 10);
          print("\n");
          load_segment(0, sect+1, size_sectors, 0x1000, CODE_SEGMENT);
        } else if(strncmp(header.old.name, "kernel.data", 11) == 0) {
          print(header.old.name);
          print(" => Length: ");
          printi(size_sectors, 10);
          print("\n");
          load_segment(0, sect+1, size_sectors, 0x5000, DATA_SEGMENT);
        }
        
        sect += size_sectors;
      } else break;
    }
    
    /*int sector;
    int i;
    int addr = 0x1000; // Read to address 0 in CODE_SEGMENT
    for(i=0 ; i<header.textsize ; i++) {
        sector = KERNEL_SECT + header.textstart + i;

        read_sector_lba_dummy(0, sector, addr, CODE_SEGMENT);
        addr += BYTES_PER_SECTOR;
    }

    addr = 0x1000; // Read to address 0 in DATA_SEGMENT
    for(i=0 ; i<header.datasize ; i++) {
        sector = KERNEL_SECT + header.datastart + i;

        read_sector_lba_dummy(0, sector, addr, DATA_SEGMENT);
        addr += BYTES_PER_SECTOR;
    }*/

    print("Loaded!\n");

    call_kernel();

    print("Done!\n");

main_end:
    for(;;) {}
}
