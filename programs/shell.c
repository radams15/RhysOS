#include <stdio.h>
#include <string.h>

int entry() {return main();}

static char* prompt = "> ";

int check_builtins(char* exe, char* args) {
	if(STREQ(exe, "clear")) {
		printf("Clearing\n");
		cls();
		return 0;
	} if(STREQ(exe, "mode")) {
		if(STREQ("40", args)) {
			set_graphics_mode(GRAPHICS_CGA_40x25);
		} else {
			set_graphics_mode(GRAPHICS_CGA_80x25);
		}
		
		return 0;
	} else if(STREQ(exe, "ls")) {
		printf("Files in directory '/'");
		return 0;
	}
	
	return 1;
}

int run_line(char* line, int length) {
	char* tok;
	char* exe;
	
	tok = strtok(line, " ");
	
	exe = tok;
	
	if(check_builtins(exe, line+strlen(exe)+1) == 0) {
		return 0;
	}
	
	exec(exe);
}

int loop() {
	char line[1024];
	
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
	
	cls();
	
	while(!ret) {
		ret = loop();
	}
	
	print("Shell exited!");

	return 0x6;
}
