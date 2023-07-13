int main();
void entry() { main(); }

#define SECTORS_PER_TRACK 18

int interrupt (int number, int AX, int BX, int CX, int DX);

void read_sector(int disk, int track, int head, int sector, int dst_addr, int dst_sector);

void read_sector_lba(int disk, int lba, int dst_addr, int dst_sector) {
	int head = (lba % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK;
	int track = (lba / (SECTORS_PER_TRACK * 2));
	int sector = (lba % SECTORS_PER_TRACK + 1);

    read_sector(disk, track, head, sector, dst_addr, dst_sector);
}

void printc(char c) {
    interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void print(char* str) {
    char* c;
    for(c = str ; *c != 0 ; c++)
        printc(*c);
}

int main() {
    char buf[512];

    print("Hello world!\r\n");
    print("Hello world 2!\r\n");

    read_sector_lba(0, 4, &buf, 0x0050);

    print(buf);

    print("Done!");

    for(;;) {}
}
