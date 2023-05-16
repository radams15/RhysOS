#include <stdio.h>

extern int main(int argc, char** argv);
int _entrypoint(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv);
void exit(char code);

#asm
export __start
export _exit

__start:
	push ax
	mov ax, [crt0_ret]
	mov [_return_addr], ax
	pop ax
	
	jmp __entrypoint
crt0_ret:
	ret
	
_exit:
	lea ax, _return_addr
	jmp ax
#endasm

void* return_addr;

int _entrypoint(int proc_stdin, int proc_stdout, int proc_stderr, int argc, char** argv) {
	int ret;
	
	stdin = proc_stdin;
	stdout = proc_stdout;
	stderr = proc_stderr;
	
	ret = main(argc, argv);
	exit(ret);
}
