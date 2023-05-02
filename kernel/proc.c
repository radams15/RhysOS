#include "proc.h"
#include "util.h"

typedef int (*proc_func)(void);

static char* proc_buf = (char*) EXE_ADDRESS;
static char* shell_buf = (char*) SHELL_ADDRESS;

int run_exe(char* buf, unsigned int size, int type, int argc, char** argv, int stdout, int stderr, int stdin) {
	int ret;
	char* out_buf;
	int stdout, stdin, stderr;
	
	switch(type) {
		case LOAD_EXE:
			out_buf = proc_buf;
			break;
		
		case LOAD_SHELL:
			out_buf = shell_buf;
			break;
		
		default:
			print_string("Unknown exec type");
			return -1;
	}
	
	memcpy(out_buf, buf, size);
	
	stdout = open("/dev/stdout");
	stdin = open("/dev/stdin");
	stderr = open("/dev/stderr");
	
	ret = ((proc_func)out_buf)(stdin, stdout, stderr, argc, argv);
	
	close(stdout);
	close(stdin);
	close(stderr);
	
	
	return ret;
}
