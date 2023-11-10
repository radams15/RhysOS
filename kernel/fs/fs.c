#include "fs.h"

#include "proc.h"
#include "util.h"

#define MAX_OPEN_FILES 64
#define MAX_MOUNTS 4
#define SECTORS_PER_TRACK 18

extern int interrupt(int number, int AX, int BX, int CX, int DX);

FsNode_t* open_files[MAX_OPEN_FILES];
FsMount_t mounts[MAX_MOUNTS] = {NULL};
FsNode_t* fs_root;

FsNode_t* fh_get_node(int fh) {
    return open_files[fh];
}

int fs_mount(const char* name, FsNode_t* parent, FsNode_t* child) {
    for (int i = 0; i < MAX_MOUNTS; i++) {
        if (mounts[i].parent == NULL && mounts[i].child == NULL) {
            strcpy(mounts[i].name, name);
            mounts[i].parent = parent;
            mounts[i].child = child;
            return 0;
        }
    }

    return 1;
}

int write(int fh, unsigned char* buffer, unsigned int size) {
    return fs_write(fh_get_node(fh), fh_get_node(fh)->offset, size, buffer);
}

int read(int fh, unsigned char* buffer, unsigned int size) {
    return fs_read(fh_get_node(fh), fh_get_node(fh)->offset, size, buffer);
}

void seek(int fh, unsigned int location) {
    fh_get_node(fh)->offset = location;
}

int create_file(char* name) {
    FsNode_t* fsnode = fs_root;
    char* tok;
    char* previous_tok = NULL;

    char buf[256];
    memcpy(&buf, name, 100);
    buf[strlen(name)] = 0;

    tok = strtok(buf, "/");

    while (tok != NULL && fsnode != NULL) {
        fsnode = fs_finddir(fsnode, tok);

        previous_tok = tok;
        tok = strtok(NULL, "/");
    }

    return NULL;
}

int open(char* name) {
    int i;
    FsNode_t* handle;

    handle = get_dir(name);

    if (handle == NULL) {
        handle = create_file(name);

        if (handle == NULL) {
            print_string("Could not find directory: '");
            print_string(name);
            print_string("'\n");
            return -1;
        }
    }

    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] == NULL) {
            open_files[i] = handle;
            seek(i, 0);
            return i;
        }
    }

    print_string("Error\n");

    return -1;
}

void close(int fh) {
    open_files[fh] = NULL;
}

FsNode_t* get_dir(char* name) {
    FsNode_t* fsnode = fs_root;
    char* tok;

    char buf[256];
    memcpy(&buf, name, strlen(name));
    buf[strlen(name)] = 0;

    tok = strtok(buf, "/");

    while (tok != NULL && fsnode != NULL) {
        for (int i = 0; i < MAX_MOUNTS; i++) {
            if (fsnode == mounts[i].parent && mounts[i].parent != NULL &&
                mounts[i].child != NULL && strcmp(tok, mounts[i].name) == 0) {
                fsnode = mounts[i].child;
                goto next;
            }
        }

        fsnode = fs_finddir(fsnode, tok);
    next:
        tok = strtok(NULL, "/");
    }
    return fsnode;
}

void read_sector(int* buffer, int sector) {
    int track = sector / (18 * 2);          // Number of tracks
    int head = (sector / 18) % 2;           // Head number (0 or 1)
    int sector_number = (sector % 18) + 1;  // Sector number (1-based)

    // Prepare the registers
    int ax = 0x0201;  // AH = 0x02 (Read), AL = 0x01 (Number of sectors to read)
    int bx = (int)buffer;  // Buffer address
    int cx =
        (track << 8) | sector_number;  // CH = Track number, CL = Sector number
    int dx =
        (head
         << 8);  // DH = Head number, DL = Drive number (e.g., floppy drive 0)

    // Call BIOS interrupt 13h
    interrupt(0x13, ax, bx, cx, dx);
}

unsigned int fs_read(FsNode_t* node,
                     unsigned int offset,
                     unsigned int size,
                     unsigned char* buffer) {
    if (node->read != 0) {
        return node->read(node, offset, size, buffer);
    }

    return 0;
}

unsigned int fs_write(FsNode_t* node,
                      unsigned int offset,
                      unsigned int size,
                      unsigned char* buffer) {
    if (node->write != 0) {
        return node->write(node, offset, size, buffer);
    }

    return 0;
}

unsigned int fs_open(FsNode_t* node, unsigned char read, unsigned char write) {
    if (node->open != 0) {
        return node->open(node, read, write);
    }

    return 0;
}

unsigned int fs_close(FsNode_t* node) {
    if (node->close != 0) {
        return node->close(node);
    }

    return 0;
}

DirEnt_t* fs_readdir(FsNode_t* node, unsigned int index) {
    if (node->readdir != 0 && (node->flags & 0x07) == FS_DIRECTORY) {
        return node->readdir(node, index);
    }

    return 0;
}

FsNode_t* fs_finddir(FsNode_t* node, char* name) {
    if (node->finddir != 0 && (node->flags & 0x07) == FS_DIRECTORY) {
        return node->finddir(node, name);
    }

    return 0;
}

void read_lba_to_segment(int disk, int lba, int dst_addr, int dst_seg) {
    int head = (lba % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK;
    int track = (lba / (SECTORS_PER_TRACK * 2));
    int sector = (lba % SECTORS_PER_TRACK + 1);

    read_sector_to_segment(disk, track, head, sector, dst_addr, dst_seg);
}
