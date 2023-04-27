#ifndef PROC_H
#define PROC_H

enum {
	LOAD_EXE,
	LOAD_SHELL
} LoadType;

int run_exe(char* buf, unsigned int size, LoadType type);

#endif
