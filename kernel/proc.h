#ifndef PROC_H
#define PROC_H

enum LoadType {
	LOAD_EXE = 0,
	LOAD_SHELL = 1
};

int run_exe(char* buf, unsigned int size, enum LoadType type, int argc, char** argv);

#endif
