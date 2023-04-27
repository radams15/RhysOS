#include "stdio.h"

int print(char* str) {
	return interrupt(0x21, 0, str, 0, 0);
}
