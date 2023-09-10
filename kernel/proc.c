#include "proc.h"
#include "util.h"

#include "fs/fat.h"
#include "fs/fs.h"

#include "serial.h"

typedef struct ExeHeader {
    char magic[2];
    short load_address;
    short segment;
    short text_size;
    short data_size;
    char _buf[512];
} ExeHeader_t;

int memcmp(char* a, char* b, int n) {
    int i;

    for (i = 0; i < n; i++) {  // If first 64 bytes are the same then program is
                               // already loaded.
        if (a[i] != b[i]) {
            return 1;
        }
    }

    return 0;  // Are the same
}

int segment_top = 0x5000;

typedef int (*prog_t)(int argc, char** argv, int in, int out, int err);
int call_0x5000(int argc, char** argv, int in, int out, int err);
int call_0x8000(int argc, char** argv, int in, int out, int err);
int call_far(int argc, char** argv, int in, int out, int err, int cs);

int exec(char* file_name, int argc, char** argv, int in, int out, int err) {
    struct FsNode* fs_node;
    int size;
    int ret;
    ExeHeader_t header;

    ProcFunc_t entry;

    fs_node = get_dir(file_name);

    if (fs_node == NULL) {
        if (file_name != NULL) {
            print_string(file_name);
            print_string(
                " is not recognised as an internal or external command.\n");
        }
        return 2;
    }

    int cluster = fs_node->start_sector;

    read_lba_to_segment(0, cluster_to_lba(cluster), &header, DATA_SEGMENT);

    if (header.magic[0] != 'R' || header.magic[1] != 'Z') {
        print_string("Invalid header magic!\n");
        return 1;
    }
    
    int segment = segment_top;
    segment_top += 0x1000;

    int addr = 0x1000;
    int sectors_read = 0;  // MEM starts @ sector 172
    for (cluster = fs_node->start_sector;
         sectors_read < header.text_size && cluster < 0xFF8;
         cluster = fat_next_cluster(cluster)) {
        read_lba_to_segment(0, cluster_to_lba(cluster), addr,
                            segment);  // Code to segment:0x1000
        addr += 512;
        sectors_read++;
    }

    addr = header.load_address;
    for (; cluster < 0xFF8; cluster = fat_next_cluster(cluster)) {
        read_lba_to_segment(0, cluster_to_lba(cluster), addr,
                            DATA_SEGMENT);  // Data to 0x3000:data_address
        addr += 512;
    }

    /*prog_t prog;

    if (header.segment == EXE_SEGMENT)
        prog = call_0x5000;
    else if (header.segment == SHELL_SEGMENT)
        prog = call_0x8000;
    else {
        print_string("Error, cannot find call segment: ");
        printi(header.segment, 16);
        print_string("\n");
        return 3;
    }

    return prog(argc, argv, in, out, err);*/
    return call_far(argc, argv, in, out, err, segment);
    
}
