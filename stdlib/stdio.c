#include "stdio.h"

int print(char* str) {
	return interrupt(0x21, 0, str, 0, 0);
}

int readline(char* buffer) {
	return interrupt(0x21, 1, buffer, 0, 0);
}
