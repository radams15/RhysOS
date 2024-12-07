#include "devfs.h"

#include "clock.h"
#include "malloc.h"
#include "rand.h"
#include "serial.h"
#include "sysinfo.h"
#include "tty.h"
#include "util.h"

#define MAX_FILES 20
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

unsigned int com_read(FsNode_t* node,
                       unsigned int offset,
                       unsigned int size,
                       unsigned char* buffer) {
    int i;

    for (i = 0; i < size; i++) {
        /* buffer[i] = serial_getc(node->meta); */
    }

    return size;
}

unsigned int com_write(FsNode_t* node,
                        unsigned int offset,
                        unsigned int size,
                        unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        /* serial_putc(node->meta, buffer[i]); */
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

int devfs_register_device_n(
        int i,
        const char* name,
        uint16_t length,
        ReadFunc read_func,
        WriteFunc write_func,
        unsigned int meta
 ) {

    if(i > MAX_FILES) {
        printf("Error, creating more DEVFS files than capable of!\n");
        return 1;
    }

    strcpyz(root_nodes[i].name, name);
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = length;
    root_nodes[i].offset = 0;
    root_nodes[i].read = read_func;
    root_nodes[i].write = write_func;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    /* root_nodes[i].meta = meta; */

    return 0;
}

int devfs_register_device(
        const char* name,
        uint16_t length,
        ReadFunc read_func,
        WriteFunc write_func
 ) {
    int out = devfs_register_device_n(num_root_nodes, name, length, read_func, write_func, 0);
    num_root_nodes++;
    return out;
}

int devfs_register_device_m(
        const char* name,
        uint16_t length,
        ReadFunc read_func,
        WriteFunc write_func,
        unsigned int meta
 ) {
    int out = devfs_register_device_n(num_root_nodes, name, length, read_func, write_func, meta);

    num_root_nodes++;
    return out;
}

void devfs_setup() {

    num_root_nodes = 0;

    devfs_register_device("stdout", 1, NULL, stdout_write);
    devfs_register_device("stdin", 1, stdin_read, NULL);
    devfs_register_device("stderr", 1, NULL, stderr_write);
    devfs_register_device("con", 1, stdin_read, stdout_write);
    devfs_register_device("lowmem", 2, lowmem_read, NULL);
    devfs_register_device("highmem", 2, highmem_read, NULL);
    devfs_register_device("graphmode", 1, graphics_mode_read, graphics_mode_write);
    /* devfs_register_device_m("com1", 1, com_read, com_write, COM1); */
    /* devfs_register_device_m("com2", 1, com_read, com_write, COM2); */
    devfs_register_device("time", 1, time_read, NULL);
    devfs_register_device("ttybg", 1, NULL, tty_bg_write);
    devfs_register_device("ttyfg", 1, NULL, tty_fg_write);
    devfs_register_device("rand", 1, rand_read, NULL);
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
    if (root_nodes == NULL) {
        printf("Failed to allocate memory for devfs\n");
        return NULL;
    }

    num_root_nodes = 0;

    devfs_setup();

    return &root_node;
}
