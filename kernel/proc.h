#ifndef PROC_H
#define PROC_H

enum {
	LOAD_EXE,
	LOAD_SHELL
} LoadType;

typedef int (*ProcFunc_t)(int stdin, int stdout, int stderr, int argc, char** argv);

int exec(char* file_name, int argc, char** argv, int in, int out, int err);

ProcFunc_t run_exe(char* buf, unsigned int size);

#endif
