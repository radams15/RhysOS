#include "proc.h"
#include "util.h"

typedef int (*proc_func)(void);

static char* proc_buf = (char*) EXE_ADDRESS;

int run_exe(char* buf, unsigned int size) {
	int ret;
	
	memcpy(proc_buf, buf, size);
	
	ret = ((proc_func)proc_buf)();
	
	return ret;
}
