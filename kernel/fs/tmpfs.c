#include "devfs.h"

#include "malloc.h"
#include "tty.h"
#include "util.h"

#define MAX_FILES 2
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SECTOR_SIZE 512

static FsNode_t root_node;
static FsNode_t* root_nodes;
static int num_root_nodes;

typedef struct Content {
    char* data;
    unsigned int length;
} Content_t;

Content_t* contents;

static DirEnt_t dirent;

DirEnt_t* tmpfs_readdir(FsNode_t* node, unsigned int index) {
    if (index >= num_root_nodes + 1) {
        return NULL;
    }

    strcpyz(dirent.name, root_nodes[index - 1].name);
    dirent.name[strlen(root_nodes[index - 1].name)] = 0;
    dirent.inode = root_nodes[index - 1].inode;

    return &dirent;
}

FsNode_t* tmpfs_finddir(FsNode_t* node, char* name) {
    int i;
    for (i = 0; i < num_root_nodes; i++) {
        if (strcmp(name, root_nodes[i].name) == 0) {
            return &root_nodes[i];
        }
    }

    return NULL;
}

unsigned int tmpfs_write(FsNode_t* node,
                         unsigned int offset,
                         unsigned int size,
                         unsigned char* buffer) {
    Content_t* content = &contents[node->inode];
    content->length += size;
    content->data = realloc(content->data, content->length);
    memcpy(content->data + node->offset, (char*)buffer, size);
    node->offset += size;

    return 0;
}

unsigned int tmpfs_read(FsNode_t* node,
                        unsigned int offset,
                        unsigned int size,
                        unsigned char* buffer) {
    Content_t* content = &contents[node->inode];
    if (size > content->length)
        size = content->length;
    memcpy((char*)buffer, content->data, size);

    return 0;
}

FsNode_t* tmpfs_create(FsNode_t* parent, const char* name) {
    int i = num_root_nodes;

    strcpyz(root_nodes[i].name, (char*)name);
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].inode = i;
    root_nodes[i].length = 1;
    root_nodes[i].offset = 0;
    root_nodes[i].read = tmpfs_read;
    root_nodes[i].write = tmpfs_write;
    root_nodes[i].create = 0;
    root_nodes[i].close = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].ref = 0;
    num_root_nodes++;

    return &root_nodes[i];
}

FsNode_t* tmpfs_init() {
    strcpyz(root_node.name, "tmp");
    root_node.flags = FS_DIRECTORY;
    root_node.inode = 0;
    root_node.length = 0;
    root_node.offset = 0;
    root_node.read = 0;
    root_node.write = 0;
    root_node.create = tmpfs_create;
    root_node.close = 0;
    root_node.readdir = tmpfs_readdir;
    root_node.finddir = tmpfs_finddir;
    root_node.ref = 0;

    root_nodes = malloc(MAX_FILES * sizeof(FsNode_t));
    contents = malloc(MAX_FILES * sizeof(Content_t));

    if (root_nodes == NULL)
        return NULL;

    num_root_nodes = 0;

    return &root_node;
}
