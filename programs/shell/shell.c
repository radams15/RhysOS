#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define MAX_PARAMS 16

int entry() {return main();}

static char* prompt = "> ";

static int run_external(char* exe, char* rest) {
	int argc;
	char* argv[MAX_PARAMS];
	char* tok;
	char* dest;
	
	argc = 1;
	argv[0] = exe;
	
	tok = strtok(rest, " ");
	
	while(tok != NULL) {	
		*(tok-1) = 0; // null terminate section (replacing space)	
		if(strcmp(tok, ">") == 0) { // redirection?
			tok = strtok(NULL, ">");
			dest = tok;
			*(tok-1) = 0;
			break;
		}
		
		argv[argc++] = tok;
		tok = strtok(NULL, " ");
	}
	
	exec(exe, argc, argv);
}

int run_line(char* line, int length) {
	char* tok;
	char* exe;
	
	tok = strtok(line, " ");
	
	exe = tok;
	
	run_external(exe, line+strlen(exe)+1);
}

int loop() {
	char line[1024];
	
	memset(&line, 0, 1024/8);
	
	printf(prompt);
	readline(line);
	printf("\n");
	
	run_line(line, sizeof(line));
	
	printf("\n");
	
	return 0;
}

int main() {
	char name[32];
	int ret = 0;
	
	//cls();
	
	while(!ret) {
		ret = loop();
	}
	
	print("Shell exited!");

	return 0x6;
}
