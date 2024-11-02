#include "proc.h"

#include "fs/fat.h"
#include "fs/fs.h"
#include "pmode.h"
#include "serial.h"
#include "tty.h"
#include "util.h"

int segments[3] = {0};  // bitmap of the segments to use in parallel

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

int segment_top = 0x4000;

int call_far(int argc,
             char** argv,
             int in,
             int out,
             int err,
             BOOL should_free,
             int cs,
             int ds);

int get_segment() {
    for (int i = 0; i < sizeof(segments); i++) {
        if (segments[i] == 0)
            return i;
    }

    return -1;
}

int exec(char* file_name,
         int argc,
         char** argv,
         int in,
         int out,
         int err,
         BOOL should_free) {
    struct FsNode* fs_node;
    int ret;
    ExeHeader_t header;

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

    read_lba_to_segment(0, cluster_to_lba(cluster), (int)&header,
                        KERNEL_SEGMENT);

    if (header.magic[0] != 'R' || header.magic[1] != 'Z') {
        print_string("Invalid header magic!\n");
        return 1;
    }

    if (header.protected_mode) {
        return pmode_exec(fs_node);
    }

    cli();  // Disable interrupts as atomic
    int segment_index = get_segment();
    if (segment_index == -1) {
        print_string("Segment allocation error in `exec`\n");
        return 1;
    }
    int segment = segment_top + (0x1000 * segment_index);
    segments[segment_index] = 1;
    sti();  // Enable interrupts

    int addr = 0x1000;
    addr -= 10;  // Remove the header.
    for (cluster = fs_node->start_sector; cluster < 0xFF8;
         cluster = fat_next_cluster(cluster)) {
        read_lba_to_segment(0, cluster_to_lba(cluster), addr,
                            segment);  // Code to segment:text
        addr += 512;
    }

    ret = call_far(argc, argv, in, out, err, should_free, segment, segment);

    cli();  // Atomic again
    segments[segment_index] = 0;
    sti();

    return ret;
}
