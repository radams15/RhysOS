#ifndef PROC_H
#define PROC_H

#include "type.h"

typedef union ExeHeader {
    struct {
        char magic[2];
        char protected_mode;
        char unused_1;
        short unused_2;
        short unused_3;
        short unused_4;
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
