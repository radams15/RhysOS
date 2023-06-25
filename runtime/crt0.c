#include <stdio.h>

extern int main(int argc, char** argv);
int _entrypoint(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv);

int _entrypoint(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv) {
	int ret;
	
	stdin = proc_stdin;
	stdout = proc_stdout;
	stderr = proc_stderr;
	
	ret = main(argc, argv);
	return ret;
}