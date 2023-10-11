#ifndef PROC_H
#define PROC_H

#include "type.h"

int exec(char* file_name, int argc, char** argv, int in, int out, int err, BOOL should_free);

void cli();
void sti();

#endif
