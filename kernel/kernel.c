#include "fs/fs.h"
#include "tty.h"
#include "proc.h"
#include "malloc.h"

#include "fs/ustar.h"
#include "fs/devfs.h"

#include "serial.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE

int stdin, stdout, stderr;

void main() {
	int err;

	err = init();

	if(err){
		print_string("\r\nError in kernel, halting!\r\n");
	}

	for(;;){}
}


int exec(char* file_name, int argc, char** argv) {
	struct FsNode* fs_node;
	char buf[SHELL_SIZE];
	int size;
	int in, out, err;
	int ret;
	ProcFunc_t entry;
	
	fs_node = get_dir(file_name);
	
	if(fs_node == NULL) {
		if(file_name != NULL) {
			print_string(file_name);
			print_string(" is not recognised as an internal or external command.\n");
		}
		return -1;
	}
	
	size = fs_read(fs_node, 0, sizeof(buf), &buf);

    entry = run_exe(&buf, size, LOAD_EXE);
    
    return entry(stdin, stdout, stderr, argc, argv);
}

int shell() {
	return exec("shell", 0, NULL);
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
		
    case 8:
		*ax = open((char*) bx);
		break;

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

void test() {
	int addr;
	int i;
	
	/*for(i=0 ; i<10 ; i++) {
		addr = malloc(100);
	}*/
}

int init(char* cmdline){	
	FsNode_t* fs_dev;
	int cursor;
	char row, col;
	
	makeInterrupt21();
	memmgr_init();
	
	//print_char('A');
	
	serial_init(COM1, BAUD_9600, PARITY_NONE, STOPBITS_ONE, DATABITS_8);
	
	//print_char('B');
	
	fs_root = ustar_init(1);
	fs_dev = devfs_init();
	ustar_mount(fs_dev, "dev");
	
	//print_char('C');
	
	stdin = open("/dev/stdin");
	stdout = open("/dev/stdout");
	stderr = open("/dev/stderr");
	
	cls();
	
	print_string("Welcome to RhysOS!\n\n");
	
	exec("mem", 0, NULL);
	print_string("\n");
	
	//test();
	
	exec("shell", 0, NULL);
	
	close(stdin);
	close(stdout);
	close(stderr);
	
	print_string("\n\nDone.");

	return 0;
}
