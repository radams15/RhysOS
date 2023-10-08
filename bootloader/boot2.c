int main();
void entry() {
    main();
}

#define SECTORS_PER_TRACK 18
#define BYTES_PER_SECTOR 512

#define FS_SECT 0

#ifndef ENABLE_SPLASH
#define ENABLE_SPLASH 1
#endif

struct SystemInfo {
    int rootfs_start;
    int highmem;
    int lowmem;
    char cmdline[256];
};

extern int ds();

union BiosBlock {
    struct {
        unsigned char bootjmp[3];
        unsigned char oem_name[8];
        unsigned int bytes_per_sector;
        unsigned char sectors_per_cluster;
        unsigned int reserved_sector_count;
        unsigned char table_count;
        unsigned int root_entry_count;
        unsigned int total_sectors_16;
        unsigned char media_type;
        unsigned int table_size_16;
        unsigned int sectors_per_track;
        unsigned int head_side_count;
        unsigned int hidden_sector_count;
        unsigned int total_sectors_32;
    };

    char raw[512];
} __attribute__((packed));

union BiosBlock bpb;

struct DirectoryEntry {
    union {
        struct {
            char name[8];
            char ext[3];
        };
        char fullname[11];
    };
    char attrib;
    char userattrib;

    char undelete;
    int createtime;
    int createdate;
    int accessdate;
    int clusterhigh;

    int modifiedtime;
    int modifieddate;
    int cluster;
    int filesize[2];
} __attribute__((packed));

unsigned int fat_table[512];
struct DirectoryEntry root_dir[32];

int interrupt(int number, int AX, int BX, int CX, int DX);
int highmem();
int lowmem();

void read_sector(int disk,
                 int track,
                 int head,
                 int sector,
                 int dst_addr,
                 int dst_seg);

void call_kernel(int ds, struct SystemInfo* info);

void read_sector_lba(int disk, int lba, int dst_addr, int dst_seg) {
    int head   = (lba % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK;
    int track  = (lba / (SECTORS_PER_TRACK * 2));
    int sector = (lba % SECTORS_PER_TRACK + 1);

    read_sector(disk, track, head, sector, dst_addr, dst_seg);
}

void print(char* str) {
    for (; *str != 0; str++) {
        if (*str == '\n')
            printc('\r');
        printc(*str);
    }
}

int read_cluster(int disk, int cluster, int dst_addr, int dst_seg) {
    //  lba_addr = cluster_begin_lba + (cluster_number - 2) *
    //  sectors_per_cluster;
    // cluster_begin_lba = = (rsvd_secs + (num_fats * 32) + root_dir_sectors)
    int cluster_start =
        33;  // bpb.reserved_sector_count + (bpb.table_count *
             // bpb.table_size_16) + bpb.root_entry_count; // TODO fix with BPB
    int lba = cluster_start + (cluster - 2) * 1;

    read_sector_lba(disk, lba, dst_addr, dst_seg);
}

int load_segment(int disk, int sect_start, int dst_addr, int dst_seg) {
    int sect;
    int addr    = dst_addr;
    int cluster = sect_start;

    while (1) {
        if (cluster >= 0xFF8)
            break;

        read_cluster(disk, cluster, dst_addr, dst_seg);

#if ENABLE_SPLASH
        printc('=');
#endif

        dst_addr += BYTES_PER_SECTOR;

        cluster = fat_table[cluster];
    }
}

int strncmp(char* a, char* b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i])
            return 1;
    }

    return 0;
}

void splash() {
    print(
        " _____  _    ___     _______ \n|  __ \\| |  | \\ \\   / / ____|\n| "
        "|__) | |__| |\\ \\_/ / (___  \n|  _  /|  __  | \\   / \\___ \\ \n| | "
        "\\ \\| |  | |  | |  ____) |\n|_|  \\_\\_|  |_|  |_| |_____/ \n        "
        "                                         \n         ____   _____ \n   "
        "     / __ \\ / ____|\n       | |  | | (___  \n       | |  | |\\___ \\ "
        "\n       | |__| |____) |\n        \\____/|_____/ \n");
    print("\n\n");
}

int strcpy(char* dst, char* src) {
    int len;
    for (len = 0; src[len] != 0; len++) {
        dst[len] = src[len];
    }

    return len;
}

int main() {
    print("Bootloader stage 2\n");

    unsigned char fat_sector[512];

    int sect = FS_SECT;
    int rootfs_start;

    read_sector_lba(0, sect, &bpb, 0x50);
    read_sector_lba(0, sect + 1, &fat_sector, 0x50);
    read_sector_lba(0, sect + 19, &root_dir, 0x50);

    unsigned char frame[3];
    int i, f1, f2, curr;
    curr = 0;
    for (i = 0; i < 512; i += 2) {
        frame[0] = fat_sector[curr];
        frame[1] = fat_sector[curr + 1];
        frame[2] = fat_sector[curr + 2];

        f1 = 0;
        f2 = 0;

        f1 |= frame[0];
        f1 &= 0x0FF;
        f1 |= (frame[1] & 0x0F) << 8;
        f2 = frame[2] << 4;
        f2 |= (frame[1] & 0xF0) >> 4;
        f2 &= 0xFFF;

        fat_table[i]     = f1;
        fat_table[i + 1] = f2;

        curr += 3;
    }

#if ENABLE_SPLASH
    splash();
#endif

    for (int i = 0; i < 32; i++) {
        struct DirectoryEntry* file = &root_dir[i];

        if (strncmp(file->fullname, "KERNEL  BIN", 11) == 0) {
#if ENABLE_SPLASH
            print("Loading kernel: [");
#endif
            load_segment(0, file->cluster, 0x1000, DATA_SEGMENT);
#if ENABLE_SPLASH
            print("]\n");
#endif
        }
    }

    print("\nKernel Loaded!\n");

    const char* cmdline = "";  // Kernel command line passed to kernel.

    struct SystemInfo info;
    info.rootfs_start = sect;
    info.highmem      = highmem();
    info.lowmem       = lowmem();
    strcpy(info.cmdline, cmdline);

    call_kernel(ds(), &info);

    for (;;) {
    }
}
