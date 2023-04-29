#ifndef RHYSOS_FS_H
#define RHYSOS_FS_H

#include "type.h"

union tar_t {
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
};

typedef struct File {
	char name[100];
	unsigned int sector_start;
	unsigned int size; // size in bytes.
} File_t;

typedef int (*FsCallback)(struct File*);

int read_file(char* buf, char* filename);
int list_directory(char* dir_name, struct File* buf);

#endif
