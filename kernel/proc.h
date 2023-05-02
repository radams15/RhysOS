#ifndef PROC_H
#define PROC_H

enum {
	LOAD_EXE,
	LOAD_SHELL
} LoadType;

int run_exe(char* buf, unsigned int size, int type, int argc, char** argv, int proc_stdin, int proc_stdout, int proc_stderr);

#endif
