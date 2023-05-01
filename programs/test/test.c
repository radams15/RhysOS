#include <stdio.h>

int main() {
	char name[32];
	
	print("\nName: ");
	readline(&name);
	print("\nHello, "); print(name); print("\n");

	return 0x6;
}
