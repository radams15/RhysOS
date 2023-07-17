int enter(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv);
int start(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv) {
  return enter(proc_stdin, proc_stdout, proc_stderr, argc, argv);
}

#include <stdio.h>

extern int main(int argc, char** argv);

int enter(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv) {
	int ret;
	
	/*stdin = proc_stdin;
	stdout = proc_stdout;
	stderr = proc_stderr;
	
	ret = main(argc, argv);
	return ret;*/
	
	stdin = 0;
	stdout = 1;
	stderr = 2;
	
	ret = main(0, NULL);
	
	asm volatile("retf");
	
	//return ret;
}
