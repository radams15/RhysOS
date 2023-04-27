#include <stdio.h>

int entry() {return main();}

static char* prompt = "> ";

int run_line(char* line, int length) {
	char* tok;
	char* exe;
	
	tok = strtok(line, " ");
	
	exe = tok;
	
	exec(exe);
	
	/*while(tok != NULL) {
		print(tok);
		print("\n");
		
		tok = strtok(NULL, " ");
	}*/
}

int loop() {
	char line[1024];
	
	print(prompt);
	readline(line);
	print("\n");
	
	run_line(line, sizeof(line));
	
	print("\n");
	
	return 0;
}

int main() {
	char name[32];
	int ret = 0;
	
	while(!ret) {
		ret = loop();
	}
	
	print("Shell exited!");

	return 0x6;
}
