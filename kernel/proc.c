#include "proc.h"
#include "util.h"

typedef struct ExeHeader {
       char type[2];
       short load_address;
} ExeHeader_t;

int run_exe(char* buf, unsigned int size) {
	int ret;
	char* out_buf;
	int stdout, stdin, stderr;
	ExeHeader_t* header = buf; // Extract the header.

	if(strcmp(header->type, "RZ") != 0) {
		print_string("Cannot execute executable of type: '");
		print_string(header->type);
		print_string("'\n");
	}

	memcpy((char*) header->load_address, buf+sizeof(ExeHeader_t), size-sizeof(ExeHeader_t)); // Only load the actual program.
	
	return (ProcFunc_t) (header->load_address);
}
