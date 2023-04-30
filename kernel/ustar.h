#ifndef USTAR_H
#define USTAR_H

#include "fs.h"

typedef union Tar {
        // Pre-POSIX.1-1988 format
        struct {
            char name[100];             // file name
            char mode[8];               // permissions
            char uid[8];                // user id (octal)
            char gid[8];                // group id (octal)
            char size[12];              // size (octal)
            char mtime[12];             // modification time (octal)
            char check[8];              // sum of unsigned characters in block, with spaces in the check field while calculation is done (octal)
            char link;                  // link indicator
            char link_name[100];        // name of linked file
        } old;

        // UStar format (POSIX IEEE P1003.1)
        struct {
            char old[156];              // first 156 octets of Pre-POSIX.1-1988 format
            char type;                  // file type
            char also_link_name[100];   // name of linked file
            char ustar[8];              // ustar\000
            char owner[32];             // user name (string)
            char group[32];             // group name (string)
            char major[8];              // device major number
            char minor[8];              // device minor number
            char prefix[155];
            struct tar_t * next;
        } new;
        
        char raw[512];
} Tar_t;

int ustar_read_file(char* buf, int n, char* filename);
int ustar_list_directory(char* dir_name, struct File* buf);

FsNode_t* ustar_init(int fs_sector_start);

#endif
