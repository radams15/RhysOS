#include "proc.h"
#include "util.h"

#include "fs/fat.h"
#include "fs/fs.h"

#include "serial.h"

int segments[3] = {0}; // bitmap of the segments to use in parallel

typedef union ExeHeader {
    struct {
        char magic[2];
        short load_address;
        short segment;
        short text_size;
        short data_size;
    };
    char buf[512];
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

int segment_top = 0x4000;

typedef int (*prog_t)(int argc, char** argv, int in, int out, int err);
int call_0x5000(int argc, char** argv, int in, int out, int err);
int call_0x8000(int argc, char** argv, int in, int out, int err);
int call_far(int argc, char** argv, int in, int out, int err, int cs, int ds);

int get_segment() {
	for(int i=0 ; i<sizeof(segments) ; i++) {
		if(segments[i] == 0)
			return i;
	}
	
	return -1;
}

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
    
    cli(); // Disable interrupts as atomic
    int segment_index = get_segment();
    if(segment_index == -1) {
    	print_string("Segment allocation error in `exec`\n");
    	return 1;
    }
    int segment = segment_top + (0x1000*segment_index);
    segments[segment_index] = 1;
    sti(); // Enable interrupts

    int addr = 0x1000;
    int sectors_read = 0;  // MEM starts @ sector 172
    for (cluster = fs_node->start_sector;
         sectors_read < header.text_size && cluster < 0xFF8;
         cluster = fat_next_cluster(cluster)) {
        read_lba_to_segment(0, cluster_to_lba(cluster), addr,
                            segment);  // Code to segment:text
        addr += 512;
        sectors_read++;
    }

    addr = header.load_address;
    for (; cluster < 0xFF8; cluster = fat_next_cluster(cluster)) {
        read_lba_to_segment(0, cluster_to_lba(cluster), addr,
                            segment);  // Data to segment:data
        addr += 512;
    }

    ret = call_far(argc, argv, in, out, err, segment, segment);
    
    cli(); // Atomic again
    segments[segment_index] = 0;
    sti();
    
    return ret;
    
}
