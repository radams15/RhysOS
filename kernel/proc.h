#ifndef PROC_H
#define PROC_H

#include "type.h"

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

int exec(char* file_name,
         int argc,
         char** argv,
         int in,
         int out,
         int err,
         BOOL should_free);

void cli();
void sti();

#endif
