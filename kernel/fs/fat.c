#include "fat.h"

#include "stdio.h"
#include "util.h"

#define MAX_FILES 64
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
struct DirectoryEntry root_dir[MAX_FILES];

static FsNode_t root_node;
static DirEnt_t dirent;
static FsNode_t fsnode;

//  lba_addr = cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
// cluster_begin_lba = = (rsvd_secs + (num_fats * 32) + root_dir_sectors)
int cluster_start =
    33;  // bpb.reserved_sector_count + (bpb.table_count * bpb.table_size_16) +
         // bpb.root_entry_count; // TODO fix with BPB

int cluster_to_lba(int cluster) {
    return cluster_start + ((cluster - 2) * 1);
}

int lba_to_cluster(int lba) {
    return ((lba - cluster_start) / 1) + 2;
}

int fat_next_cluster(int prev_cluster) {
    return fat_table[prev_cluster];
}

int fat_next_lba(int prev_lba) {
    int cluster = lba_to_cluster(prev_lba);
    int next_cluster = fat_table[cluster];

    if (next_cluster >= 0xFF8)
        return 0;

    return cluster_to_lba(next_cluster);
}

unsigned int fat_read(FsNode_t* node,
                      unsigned int byte_offset,
                      unsigned int byte_size,
                      unsigned char* out_buffer) {
    signed int cluster;
    unsigned int sector_offset;
    unsigned int sector_bytes;
    unsigned int bytes_read = 0;
    unsigned int end_byte_offset;
    unsigned char temp_buffer[SECTOR_SIZE];
    int i;

    end_byte_offset = byte_offset + byte_size;

    if (end_byte_offset > node->length) {
        byte_size = node->length - byte_offset;
    }

    if (byte_size <= 0) {
        return 0;
    }

    cluster = node->start_sector;

    int cluster_offset = byte_offset / SECTOR_SIZE;

    for (int i = 0; i < cluster_offset; i++) {
        if (cluster >= 0xFF8)
            return 0;

        cluster = fat_table[cluster];
    }

    sector_offset = byte_offset % SECTOR_SIZE;

    while (bytes_read < byte_size) {
        if (cluster >= 0xFF8)
            break;

        read_sector(&temp_buffer, cluster_to_lba(cluster));

        sector_bytes = MIN(SECTOR_SIZE - sector_offset, byte_size - bytes_read);

        memcpy(out_buffer + bytes_read, temp_buffer + sector_offset,
               sector_bytes);

        bytes_read += sector_bytes;
        cluster = fat_table[cluster];
        sector_offset = 0;
    }

    return bytes_read;
}

/*FsNode_t* fat_create(char* name) {
    int start_cluster;
    for (int i = 0; i < 512; i++) {
        if (fat_table[i] == 0) {
            fat_table[i] = 0xFF8;
            start_cluster = i;
            break;
        }
    }

    int inode;
    FsNode_t* out = NULL;
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_nodes[i].name[0] == 0) {
            printi(i, 10);
            out = &root_nodes[i];
            inode = i;
            break;
        }
    }

    if (out == NULL) {
        print_string("Failed to create file!\n");
        return NULL;
    }

    strcpy(out->name, name);
    out->start_sector = start_cluster;
    out->inode = inode;
    out->flags = FS_FILE;
    out->length = 0;
    out->offset = 0;
    out->read = fat_read;
    out->write = 0;
    out->open = 0;
    out->close = 0;
    out->readdir = 0;
    out->finddir = 0;
    out->ref = 0;

    num_root_nodes++;

    return out;
}*/

DirEnt_t* fat_readdir(FsNode_t* node, unsigned int i) {
    struct DirectoryEntry* entry = &root_dir[i];
    
    if(entry->cluster == 0)
        return NULL;
    
    memcpy(dirent.name, entry->name, 8);

    int x;

    if (entry->ext[0] != ' ') {
        for (x = 0; x < 8; x++) {
            if (dirent.name[x] == ' ') {
                break;
            }
        }
        dirent.name[x] = '.';

        memcpy(dirent.name + x + 1, entry->ext, 3);
    }

    dirent.name[x + 4] = 0;

    for (int x = 0; x < 11; x++) {
        if (dirent.name[x] >= 65 &&
            dirent.name[x] <=
                90)  // Convert uppercase => lowercase
            dirent.name[x] += 32;
    }
    dirent.inode = fsnode.inode;
    
    return &dirent;
}

FsNode_t* fat_finddir(FsNode_t* node, char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        struct DirectoryEntry* entry = &root_dir[i];
        
        for(int x=0 ; x<FILE_NAME_MAX ; x++) {
            fsnode.name[x] = 0;
        }

        if (entry->name[0] != NULL) {
            memcpy(fsnode.name, entry->name, 8);

            int x;

            if (entry->ext[0] != ' ') {
                for (x = 0; x < 8; x++) {
                    if (fsnode.name[x] == ' ') {
                        break;
                    }
                }
                fsnode.name[x] = '.';

                memcpy(fsnode.name + x + 1, entry->ext, 3);
            }

            fsnode.name[x + 4] = 0;

            for (int x = 0; x < 11; x++) {
                if (fsnode.name[x] >= 65 &&
                    fsnode.name[x] <=
                        90)  // Convert uppercase => lowercase
                    fsnode.name[x] += 32;
            }
            
            if (strcmp(name, fsnode.name) == 0) {
                fsnode.flags = FS_FILE;
                fsnode.inode = i;
                fsnode.start_sector = entry->cluster;
                fsnode.length = entry->filesize[0];
                fsnode.offset = 0;
                fsnode.read = fat_read;
                fsnode.write = 0;
                fsnode.open = 0;
                fsnode.close = 0;
                fsnode.readdir = 0;
                fsnode.finddir = 0;
                fsnode.ref = 0;
                return &fsnode;
            }
        }
    }
    
    return NULL;
}

void init_fat_table(int sector_start) {
    unsigned char fat_sector[512];

    sector_start = 1;  // TODO: Fix

    read_lba_to_segment(0, sector_start, &fat_sector, DATA_SEGMENT);
    read_lba_to_segment(0, sector_start + 18, &root_dir, DATA_SEGMENT);
    read_lba_to_segment(0, sector_start + 19, &root_dir[16], DATA_SEGMENT);

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

        fat_table[i] = f1;
        fat_table[i + 1] = f2;

        curr += 3;
    }
}

FsNode_t* fat_init(int sector_start) {
    for (int i = 0; i < MAX_FILES; i++) {
        root_dir[i].name[0] = NULL;
    }
    
    init_fat_table(sector_start);

    strcpy(root_node.name, "/");
    root_node.flags = FS_DIRECTORY;
    root_node.inode = 0;
    root_node.length = 0;
    root_node.offset = 0;
    root_node.read = 0;
    root_node.write = 0;
    root_node.open = 0;
    root_node.close = 0;
    root_node.readdir = fat_readdir;
    root_node.finddir = fat_finddir;
    root_node.ref = 0;

    return &root_node;
}
