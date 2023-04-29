#include <stdio.h>

int main() {
	char file[32];
	char buffer[1024];
	
	printf("\nFile: ");
	readline(&file);
	printf("\n");
	
	printf("OK!\n");
	
	return 0;
}
