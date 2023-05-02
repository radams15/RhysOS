#include <stdio.h>

extern int main(int argc, char** argv);

int _start(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv) {
	stdin = proc_stdin;
	stdout = proc_stdout;
	stderr = proc_stderr;
	
	return main(argc, argv);
}
