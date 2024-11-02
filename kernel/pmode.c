#include "pmode.h"
#include "fs/fat.h"
#include "fs/fs.h"
#include "tty.h"

int call_pmode();

int pmode_exec(struct FsNode* fs_node) {
    ExeHeader_t header;

    if (fs_node == NULL) {
        return 2;
    }

    int cluster = fs_node->start_sector;

    read_lba_to_segment(0, cluster_to_lba(cluster), (int)&header,
                        KERNEL_SEGMENT);

    if (header.magic[0] != 'R' || header.magic[1] != 'Z') {
        print_string("Invalid header magic!\n");
        return 1;
    }

    int segment = 0x0;

    int addr = 0x1000;
    addr -= 10;  // Remove the header.
    for (cluster = fs_node->start_sector; cluster < 0xFF8;
         cluster = fat_next_cluster(cluster)) {
        read_lba_to_segment(0, cluster_to_lba(cluster), addr,
                            segment);  // Code to segment:text
        addr += 512;
    }

    call_pmode();

    return 0;
}
