#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define MAX_PARAMS 16

int entry() {return main();}

static char* prompt = "> ";

void dir_listing(char* dir) {
	int i;
	File_t dir_buf[20]; // allow for 20 files max.
	File_t* file;

	printf("Files in directory '/'");
	dir_list(dir, &dir_buf);
	
	for(i=0 ; i<20 ; i++) {
		file = &dir_buf[i];
		
		if(strlen(file->name) == 0)
			break;

		printf("- %s\n", file->name);
	}
}

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
	} else if(STREQ(exe, "dir")) {
		dir_listing("/");
		return 0;
	}
	
	return 1;
}

static int run_external(char* exe, char* rest) {
	int argc;
	char* argv[MAX_PARAMS];
	char* tok;
	
	argc = 1;
	argv[0] = exe;
	
	tok = strtok(rest, " ");
	
	while(tok != NULL) {		
		*(tok-1) = 0; // null terminate section (replacing space)		
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
	
	if(check_builtins(exe, line+strlen(exe)+1) == 0) {
		return 0;
	}
	
	run_external(exe, line+strlen(exe)+1);
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
