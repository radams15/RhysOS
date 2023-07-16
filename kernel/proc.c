#include "proc.h"
#include "util.h"

#include "serial.h"

typedef struct ExeHeader {
	char type[2];
	short load_address;
} ExeHeader_t;

int memcmp(char* a, char* b, int n) {
    int i;
    
    for(i=0 ; i<n ; i++) { // If first 64 bytes are the same then program is already loaded.
        if(a[i] != b[i]) {
          return 1;
        }
    }
    
    return 0; // Are the same
}

ProcFunc_t run_exe(char* buf, unsigned int size) {
	int ret;
	char* dst_mem;
	ExeHeader_t* header;
	
	header = buf; // Extract the header.
	
	if(strcmp(header->type, "RZ") != 0) {
		print_string("Cannot execute executable of type: '");
		print_string(header->type);
		print_string("'\n");
	}
	
	dst_mem = (char*) header->load_address;
	
	if(memcmp(buf+sizeof(ExeHeader_t), dst_mem, 64) != 0) {
    	  memcpy(dst_mem, buf+sizeof(ExeHeader_t), size-sizeof(ExeHeader_t)); // Only load the actual program.
	}

	return (ProcFunc_t) (header->load_address);
}
