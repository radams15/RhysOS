#include "fs/fs.h"
#include "tty.h"
#include "proc.h"

#include "fs/ustar.h"
#include "devfs.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE

FsNode_t* fs_root;
FsNode_t* fs_dev;

void main() {
	int err;

	err = init();

	if(err){
		print_string("\r\nError in kernel, halting!\r\n");
	}

	for(;;){}
}

int get_dir(char* name) {
	FsNode_t* fsnode = fs_root;
	char* tok;
	
	char buf[256];
	memcpy(&buf, name, 100);
	buf[strlen(name)] = 0;
	
	tok = strtok(buf, "/");
	
	while(tok != NULL) {
		fsnode = fs_finddir(fsnode, tok);
		
		tok = strtok(NULL, "/");
	}
	
	return fsnode;
}

int shell() {
	struct FsNode* fs_node;
	char buf[SHELL_SIZE];
	int size;
	
	fs_node = fs_finddir(fs_root, "shell");
	
	if(fs_node == NULL) {
		print_string("Failed to find shell!\n");
		return;
	}
	
	size = fs_read(fs_node, 0, sizeof(buf), &buf);
    
    return run_exe(&buf, size, LOAD_SHELL, 0, NULL);
}

int exec(char* file_name, int argc, char** argv) {
	struct FsNode* fs_node;
	char buf[SHELL_SIZE];
	int size;
	
	fs_node = get_dir(file_name);
	
	if(fs_node == NULL) {
		print_string("Failed to find shell!\n");
		return;
	}
	
	size = fs_read(fs_node, 0, sizeof(buf), &buf);
    
    return run_exe(&buf, size, LOAD_EXE, argc, argv);
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

int handleInterrupt21(int ax, int bx, int cx, int dx) {
  switch(ax) {
    case 0:
		print_string((char *)bx);
		break;
		
    case 1:
		print_char((char) bx);
		break;

    case 2:
		readline(bx);
		break;

    case 3:
		exec(bx, cx, dx);
		break;

    case 4:
		set_graphics_mode(bx);
		break;

    case 5:
		list_directory(bx, cx);
		break;

    case 6:
		read_file((char*) bx, (int) cx, (char*) dx);
		break;
	
    case 7:
		write_file((char*) bx, (int) cx, (char*) dx);
		break;

    default:
		print_string("Unknown interrupt: ");
		print_hex_4(ax);
		print_string("!\r\n");
		break;
  }
}

int init(){	
	makeInterrupt21();
	
	fs_root = ustar_init(1);
	fs_dev = devfs_init();
	ustar_mount(fs_dev, "dev");
	
	shell();
	
	print_string("\n\nDone.");

	return 0;
}
