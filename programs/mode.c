#include <stdio.h>
#include <string.h>
#include <syscall.h>

int main(int argc, char** argv) {		
	if(argc != 2) {
		printf("Usage: %s [MODE]\n", argv[0]);
		return 1;
	} 
	
	if(STREQ("40", argv[1])) {
		set_graphics_mode(GRAPHICS_CGA_40x25);
	} else {
		set_graphics_mode(GRAPHICS_CGA_80x25);
	}
	
	return 0;
}
