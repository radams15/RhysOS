#include "devfs.h"

#include "tty.h"
#include "util.h"

#include "serial.h"

#define MAX_FILES 32
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SECTOR_SIZE 512

static FsNode_t root_node;
static FsNode_t root_nodes[MAX_FILES];
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

void stdout_write(FsNode_t* node,
                  unsigned int offset,
                  unsigned int size,
                  unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        print_char(buffer[i]);
    }
}

void stderr_write(FsNode_t* node,
                  unsigned int offset,
                  unsigned int size,
                  unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        print_char_colour(buffer[i], 0x0, 0xC);
    }
}

void stdin_read(FsNode_t* node,
                unsigned int offset,
                unsigned int size,
                unsigned char* buffer) {
    // unsigned char* top;
    int i;

    for (i = 0; i < size; i++) {
        buffer[i] = getch();
    }
    /*char c;

    while(buffer < top+size) {
            c = getch();

            if(c == 0x8) { // backspace
                    *(buffer--) = ' ';
            } else {
                    *(buffer++) = c;
            }
    }

    *(buffer++) = 0; // null-terminate*/
}

void com1_read(FsNode_t* node,
               unsigned int offset,
               unsigned int size,
               unsigned char* buffer) {
    int i;

    for (i = 0; i < size; i++) {
        buffer[i] = serial_getc(COM1);
    }
}

void com1_write(FsNode_t* node,
                unsigned int offset,
                unsigned int size,
                unsigned char* buffer) {
    int i;

    for (i = offset; i < size; i++) {
        serial_putc(COM1, buffer[i]);
    }
}

void int2chars(unsigned int in, unsigned char* buffer) {
    buffer[0] = (unsigned char)in & 0xff;  // low byte
    buffer[1] = (in >> 8) & 0xff;          // high byte
}

int chars2int(unsigned char* buffer) {
    return (buffer[1] << 8) | buffer[0];
}

void lowmem_read(FsNode_t* node,
                 unsigned int offset,
                 unsigned int size,
                 unsigned char* buffer) {
    unsigned int mem = lowmem();
    int2chars(mem, buffer);
}

void highmem_read(FsNode_t* node,
                  unsigned int offset,
                  unsigned int size,
                  unsigned char* buffer) {
    int mem = highmem();
    int2chars(mem, buffer);
}

void mem_read(FsNode_t* node,
              unsigned int offset,
              unsigned int size,
              unsigned char* buffer) {
    int low = lowmem();
    int high = highmem();
    int2chars(low + high, buffer);
}

void fda_read(FsNode_t* node,
              unsigned int byte_offset,
              unsigned int byte_size,
              unsigned char* out_buffer) {
    signed int start_sector;
    unsigned int sector_offset;
    unsigned int sector_bytes;
    unsigned int bytes_read = 0;
    unsigned int end_byte_offset;
    unsigned char temp_buffer[SECTOR_SIZE];
    int i;

    end_byte_offset = byte_offset + byte_size;

    if (byte_size <= 0) {
        return;
    }

    start_sector = byte_offset / SECTOR_SIZE;
    sector_offset = byte_offset % SECTOR_SIZE;

    while (bytes_read < byte_size) {
        read_sector(&temp_buffer, start_sector);

        sector_bytes = MIN(SECTOR_SIZE - sector_offset, byte_size - bytes_read);

        memcpy(out_buffer + bytes_read, temp_buffer + sector_offset,
               sector_bytes);

        bytes_read += sector_bytes;
        start_sector++;
        sector_offset = 0;
    }

    out_buffer[bytes_read] = 0;  // Null terminate, just in case it's a string.
}

void graphics_mode_read(FsNode_t* node,
                        unsigned int byte_offset,
                        unsigned int byte_size,
                        unsigned char* out_buffer) {
    int mode = get_graphics_mode();
    int2chars(mode, out_buffer);
}

void graphics_mode_write(FsNode_t* node,
                         unsigned int offset,
                         unsigned int size,
                         unsigned char* buffer) {
    int mode = chars2int(buffer);
    set_graphics_mode(mode);
}

void time_read(FsNode_t* node,
               unsigned int byte_offset,
               unsigned int byte_size,
               unsigned char* out_buffer) {
    time(out_buffer);
}

void tty_fg_write(FsNode_t* node,
                  unsigned int offset,
                  unsigned int size,
                  unsigned char* buffer) {
    set_fg(buffer[0]);
}

void tty_bg_write(FsNode_t* node,
                  unsigned int offset,
                  unsigned int size,
                  unsigned char* buffer) {
    set_bg(buffer[0]);
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "mem");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 2;
    root_nodes[i].offset = 0;
    root_nodes[i].read = mem_read;
    root_nodes[i].write = 0;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    i++;

    strcpyz(root_nodes[i].name, "fda");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = fda_read;
    root_nodes[i].write = 0;
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
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
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    /*i++;

    strcpyz(root_nodes[i].name, "ttybg");
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = tty_bg_write;
    root_nodes[i].write = 0;
    root_nodes[i].open = 0;
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
    root_nodes[i].read = tty_fg_write;
    root_nodes[i].write = 0;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;*/
}

FsNode_t* devfs_init() {
    strcpyz(root_node.name, "dev");
    root_node.flags = FS_DIRECTORY;
    root_node.inode = 0;
    root_node.length = 0;
    root_node.offset = 0;
    root_node.read = 0;
    root_node.write = 0;
    root_node.open = 0;
    root_node.close = 0;
    root_node.readdir = devfs_readdir;
    root_node.finddir = devfs_finddir;
    root_node.ref = 0;

    num_root_nodes = 0;

    devfs_setup();

    return &root_node;
}
