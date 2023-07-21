void main();
void entry(){main();}

#include "fs/fs.h"
#include "tty.h"
#include "proc.h"
#include "malloc.h"

#include "fs/ustar.h"
#include "fs/devfs.h"

#include "graphics.h"
#include "clock.h"
#include "serial.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE

int stdin, stdout, stderr;

int init();

void main() {
	int err;

	err = init();

	if(err){
		print_string("\r\nError in kernel, halting!\r\n");
	}

	for(;;){}
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
	int count = 0;
	DirEnt_t* node = NULL;
	FsNode_t* fsnode;
	FsNode_t* root = get_dir(dir_name);
	
	if(root == NULL) {
		print_string("Cannot find directory!\n");
		return 0;
	}
	
	while ( (node = fs_readdir(root, i)) != NULL) {
		fsnode = fs_finddir(root, node->name);
		if(fsnode != NULL) {
			if(buf != NULL)
				memcpy(buf++, fsnode, sizeof(FsNode_t));
			count++;
		}
		
		i++;
	}

	return count;
}

typedef struct SyscallArgs {
  int num; // Syscall number
  int a, b, c, d, e, f;
  int ds; // Data segment
} SyscallArgs_t;

int handleInterrupt21(int* ax, int bx, int cx, int* dx) {
  SyscallArgs_t* args = ax;
  //int ds = dx[0];
  //print_string("DS: '"); printi(ds, 16); print_string("'\n");
  
  switch(*ax) {
    case 3:
		args->num = exec(args->a, args->b, args->c, args->d, args->e, args->f);
		break;

    case 5:
		args->num = list_directory(args->a, args->b);
		break;

    case 6:
		args->num = read(args->a, args->b, args->c);
		break;
	
    case 7:
		args->num = write(args->a, args->b, args->c);
		break;
		
    case 8:
		args->num = open(args->a);
		break;

    case 9:
		close(args->a);
		break;
		
    case 10:
		seek(args->a, args->b);
		break;

    default:
		print_string("Unknown interrupt: ");
		print_hex_4(args->a);
		print_string("!\r\n");
		args->num = -1;
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
	} else {
		print_string("A20 line is unavaiable\n");
	}
}

void test() {
	int i;
	int len;
	FsNode_t dir_buf[16];
	FsNode_t* file;

	for(i=0 ; i<16 ; i++) {
		dir_buf[i].name[0] = 0;
	}

	len = list_directory("/dev", dir_buf);
	
	for(i=0 ; i<len ; i++) {		
		file = &dir_buf[i];
		
		if(file == NULL)
			break;

		print_string("\t - "); print_string(file->name); print_string("\n");
	}
}

int init(){		
	FsNode_t* fs_dev;
	
	a20_init();
	
	memmgr_init();
	print_string("Memory manager enabled\n");
	
	makeInterrupt21();
	print_string("Int 21h enabled\n");
	
	rtc_init();
	print_string("RTC enabled\n");
	
	serial_init(COM1, BAUD_9600, PARITY_NONE, STOPBITS_ONE, DATABITS_8);
	print_string("/dev/COM1 enabled\n");

	fs_root = ustar_init(1);
	fs_dev = devfs_init();
	ustar_mount(fs_dev, "dev");
	print_string("Root filesystem mounted\n");
			
	stdin = open("/dev/stdin");
	stdout = open("/dev/stdout");
	stderr = open("/dev/stderr");
	
	cls();
	
	print_string("Welcome to RhysOS!\n\n");
	
	exec("mem", 0, NULL, stdin, stdout, stderr);
	print_string("\n");
	exec("shell", 0, NULL, stdin, stdout, stderr);
	
	close(stdin);
	close(stdout);
	close(stderr);
	
	print_string("\n\nDone.");

	return 0;
}
