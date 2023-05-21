#ifndef PROC_H
#define PROC_H

typedef int (*ProcFunc_t)(int, int, int, int, char**);

int run_exe(char* buf, unsigned int size);

#endif
