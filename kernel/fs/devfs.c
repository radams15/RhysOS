#include "devfs.h"

#include "clock.h"
#include "malloc.h"
#include "rand.h"
#include "serial.h"
#include "sysinfo.h"
#include "tty.h"
#include "util.h"

#define MAX_FILES 16
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SECTOR_SIZE 512

static FsNode_t root_node;
static FsNode_t* root_nodes;
static int num_root_nodes;

static DirEnt_t dirent;

DirEnt_t* devfs_readdir(FsNode_t* node, unsigned int index) {
    if (index >= num_root_nodes + 1) {
        return NULL;
    }

    strcpyz(dirent.name, root_nodes[index - 1].name);
    dirent.name[strlen(root_nodes[index - 1].name)] = 0;
    dirent.inode = root_nodes[index - 1].inode;

    return &dirent;
}

FsNode_t* devfs_finddir(FsNode_t* node, char* name) {
    int i;
    for (i = 0; i < num_root_nodes; i++) {
        if (strcmp(name, root_nodes[i].name) == 0) {
            return &root_nodes[i];
        }
    }

    return NULL;
}

unsigned int stdout_write(FsNode_t* node,
                          unsigned int offset,
                          unsigned int size,
                          unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        print_char(buffer[i]);
    }

    return size;
}

unsigned int stderr_write(FsNode_t* node,
                          unsigned int offset,
                          unsigned int size,
                          unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        print_char_colour(buffer[i], 0x0, 0xC);
    }

    return size;
}

unsigned int stdin_read(FsNode_t* node,
                        unsigned int offset,
                        unsigned int size,
                        unsigned char* buffer) {
    int i;

    for (i = 0; i < size; i++) {
        buffer[i] = ngetch();
    }

    return size;
}

unsigned int com1_read(FsNode_t* node,
                       unsigned int offset,
                       unsigned int size,
                       unsigned char* buffer) {
    int i;

    for (i = 0; i < size; i++) {
        buffer[i] = serial_getc(COM1);
    }

    return size;
}

unsigned int com1_write(FsNode_t* node,
                        unsigned int offset,
                        unsigned int size,
                        unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        serial_putc(COM1, buffer[i]);
    }

    return size;
}

unsigned int com2_read(FsNode_t* node,
                       unsigned int offset,
                       unsigned int size,
                       unsigned char* buffer) {
    int i;

    for (i = 0; i < size; i++) {
        buffer[i] = serial_getc(COM2);
    }

    return size;
}

unsigned int com2_write(FsNode_t* node,
                        unsigned int offset,
                        unsigned int size,
                        unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        serial_putc(COM2, buffer[i]);
    }

    return size;
}

void int2chars(unsigned int in, unsigned char* buffer) {
    buffer[0] = (unsigned char)in & 0xff;  // low byte
    buffer[1] = (in >> 8) & 0xff;          // high byte
}

int chars2int(unsigned char* buffer) {
    return (buffer[1] << 8) | buffer[0];
}

unsigned int lowmem_read(FsNode_t* node,
                         unsigned int offset,
                         unsigned int size,
                         unsigned char* buffer) {
    int mem = lowmem();
    int2chars(mem, buffer);
    return 2;
}

unsigned int highmem_read(FsNode_t* node,
                          unsigned int offset,
                          unsigned int size,
                          unsigned char* buffer) {
    int mem = highmem();
    int2chars(mem, buffer);
    return 2;
}

unsigned int graphics_mode_read(FsNode_t* node,
                                unsigned int byte_offset,
                                unsigned int byte_size,
                                unsigned char* out_buffer) {
    int mode = get_graphics_mode();
    int2chars(mode, out_buffer);
    return 2;
}

unsigned int graphics_mode_write(FsNode_t* node,
                                 unsigned int offset,
                                 unsigned int size,
                                 unsigned char* buffer) {
    set_graphics_mode(buffer[0], buffer[1]);

    return 2;
}

unsigned int time_read(FsNode_t* node,
                       unsigned int byte_offset,
                       unsigned int byte_size,
                       unsigned char* out_buffer) {
    time((TimeDelta_t*)out_buffer);
    return 2;
}

unsigned int tty_fg_write(FsNode_t* node,
                          unsigned int offset,
                          unsigned int size,
                          unsigned char* buffer) {
    set_fg(buffer[0]);
    return size;
}

unsigned int tty_bg_write(FsNode_t* node,
                          unsigned int offset,
                          unsigned int size,
                          unsigned char* buffer) {
    set_bg(buffer[0]);
    return size;
}

unsigned int rand_read(FsNode_t* node,
                       unsigned int byte_offset,
                       unsigned int byte_size,
                       unsigned char* out_buffer) {
    int out = rand();
    int2chars(out, out_buffer);
    return 2;
}

void devfs_setup() {
    int i = 0;

    strcpyz(root_nodes[i].name, "stdout");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = 0;
    root_nodes[i].write = stdout_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "stdin");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = stdin_read;
    root_nodes[i].write = 0;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "stderr");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = 0;
    root_nodes[i].write = stderr_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "con");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = stdin_read;
    root_nodes[i].write = stdout_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "highmem");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 2;
    root_nodes[i].offset = 0;
    root_nodes[i].read = highmem_read;
    root_nodes[i].write = 0;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "lowmem");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 2;
    root_nodes[i].offset = 0;
    root_nodes[i].read = lowmem_read;
    root_nodes[i].write = 0;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "graphmode");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = graphics_mode_read;
    root_nodes[i].write = graphics_mode_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "com1");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = com1_read;
    root_nodes[i].write = com1_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "com2");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = com2_read;
    root_nodes[i].write = com2_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "time");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = time_read;
    root_nodes[i].write = 0;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "ttybg");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = 0;
    root_nodes[i].write = tty_bg_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "ttyfg");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = 0;
    root_nodes[i].write = tty_fg_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "rand");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = rand_read;
    root_nodes[i].write = 0;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;
}

FsNode_t* devfs_init() {
    strcpyz(root_node.name, "dev");
    root_node.flags = FS_DIRECTORY;
    root_node.inode = 0;
    root_node.length = 0;
    root_node.offset = 0;
    root_node.read = 0;
    root_node.write = 0;
    root_node.create = 0;
    root_node.close = 0;
    root_node.readdir = devfs_readdir;
    root_node.finddir = devfs_finddir;
    root_node.ref = 0;

    root_nodes = malloc(MAX_FILES * sizeof(FsNode_t));

    if (root_nodes == NULL)
        return NULL;

    num_root_nodes = 0;

    devfs_setup();

    return &root_node;
}
