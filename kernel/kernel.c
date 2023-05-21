#include "fs/fs.h"
#include "tty.h"
#include "proc.h"

#include "fs/ustar.h"
#include "fs/devfs.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE

int stdin, stdout, stderr;

void main() {
	int err;

	print_string("RhysOS Initialising!\n");

	err = init();

	if(err){
		print_string("\r\nError in kernel, halting!\r\n");
	}

	/*for(;;){
		print_string("RhysOS Initialising!\n");
	}*/
}

int exec(char* file_name, int argc, char** argv) {
	struct FsNode* fs_node;
	char buf[SHELL_SIZE];
	int size;
	ProcFunc_t entry;
	
	fs_node = get_dir(file_name);
	
	if(fs_node == NULL) {
		print_string(file_name);
		print_string(" is not recognised as an internal or external command.\n");
		return -1;
	}
	
	size = fs_read(fs_node, 0, sizeof(buf), &buf);
    
	entry = run_exe(&buf, size);
     
	return entry(stdin, stdout, stderr, argc, argv);
}

int read_file(char* buf, int n, char* file_name) {
	struct FsNode* fs_node;
	int size;
	
	fs_node = get_dir(file_name);
	
	if(fs_node == NULL) {
		print_string("Failed to find file!\n");
		return -1;
	}
	
	size = fs_read(fs_node, 0, n, buf);
    
    return size;
}

int write_file(char* buf, int n, char* file_name) {
	struct FsNode* fs_node;
	int size;
	
	fs_node = get_dir(file_name);
	
	if(fs_node == NULL) {
		print_string("Failed to find file!\n");
		return -1;
	}
	
	size = fs_write(fs_node, 0, n, buf);
    
    return size;
}

int list_directory(char* dir_name, FsNode_t* buf) {
	int i = 0;
	DirEnt_t* node = 0;
	FsNode_t* fsnode;
	FsNode_t* root = get_dir(dir_name);
	
	if(root == NULL) {
		print_string("Cannot find file!\n");
		return -1;
	}
	
	while ( (node = fs_readdir(root, i)) != NULL) {
		fsnode = fs_finddir(root, node->name);
		if(fsnode != NULL) {
			memcpy(buf++, fsnode, sizeof(FsNode_t));
		}
		
		i++;
	}
	
	return i;
}

int handleInterrupt21(int* ax, int bx, int cx, int dx) {
  switch(*ax) {
    case 3:
		*ax = exec(bx, cx, dx);
		break;

    case 5:
		*ax = list_directory(bx, cx);
		break;

    case 6:
		*ax = read((int) bx, (char*) cx, (int) dx);
		break;
	
    case 7:
		*ax = write((int) bx, (char*) cx, (int) dx);
		break;
		
    case 8: {
		*ax = open((char*) bx);
		break;
	}
	
    case 9:
		close((char*) bx);
		break;
		
    case 10:
		seek((char*) bx, (int) cx);
		break;

    default:
		print_string("Unknown interrupt: ");
		print_hex_4(*ax);
		print_string("!\r\n");
		*ax = -1;
		break;
  }
}

void a20_init() {
       if(a20_available()) {
               int enable_fail;
               print_string("A20 line is available\n");
               enable_fail = a20_enable();
               
               if(enable_fail)
                       print_string("A20 line failed to enable\n");
               else
                       print_string("A20 line successfully enabled\n");
       } else
               print_string("A20 line is unavaiable\n");
}


int init(){	
	FsNode_t* fs_dev;
	int cursor;
	char row, col;
	
	a20_init();
	
	memmgr_init();
	
	makeInterrupt21();

	fs_root = ustar_init(1);
	fs_dev = devfs_init();
	ustar_mount(fs_dev, "dev");

	cls();
	
	stdin = open("/dev/stdin");
	stdout = open("/dev/stdout");
	stderr = open("/dev/stderr");

	exec("/mem", 0, 0);
	exec("/shell", 0, 0);
	
	print_string("\n\nDone.");

	return 0;
}
