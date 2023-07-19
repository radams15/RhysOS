#include "proc.h"
#include "util.h"

#include "fs/fs.h"

#include "serial.h"

typedef struct ExeHeader {
  char magic[2];
  short load_address;
  short segment;
  short text_size;
  short data_size;
  char _buf[512];
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

typedef int (*prog_t)(int argc, char** argv, int in, int out, int err);

int call_0x5000(int argc, char** argv, int in, int out, int err);
int call_0x8000(int argc, char** argv, int in, int out, int err);

int exec(char* file_name, int argc, char** argv, int in, int out, int err) {
	struct FsNode* fs_node;
	int size;
	int ret;
	ExeHeader_t header;
	
	ProcFunc_t entry;
	
	fs_node = get_dir(file_name);
	
	if(fs_node == NULL) {
		if(file_name != NULL) {
			print_string(file_name);
			print_string(" is not recognised as an internal or external command.\n");
		}
		return 2;
	}
	
	read_lba_to_segment(0, fs_node->start_sector, &header, 0x3000);
	
	int addr = 0x1000;
	int sec;
	for(sec=fs_node->start_sector ; sec < fs_node->start_sector+header.text_size ; sec++) {
		read_lba_to_segment(0, sec, addr, header.segment); // Code to segment:0x1000
		addr += 512;
	}
	
	int end_text = sec;
	
	addr = header.load_address;
	for(sec=end_text ; sec < end_text+header.data_size ; sec++) {
		read_lba_to_segment(0, sec, addr, 0x3000); // Data to 0x3000:0x7000
		addr += 512;
	}

    prog_t prog;
    
    if(header.segment == 0x5000)
      prog = call_0x5000;
    else if(header.segment == 0x8000)
      prog = call_0x8000;
    else {
      print_string("Error, cannot find call segment!\n");
      return 3;
    }

    return prog(argc, argv, in, out, err);
}
