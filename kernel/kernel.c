#include "fs.h"
#include "tty.h"
#include "proc.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE

void main();
void entry() {main();}

void main() {
	int err;

	err = init();

	if(err){
		print_string("\r\nError in kernel, halting!\r\n");
	}

	for(;;){}
}

int shell() {
	int ret;
	int size;
	char buf[SHELL_SIZE];
    
    read_file(&buf, SHELL_SIZE, "/shell.bin");
    
    return run_exe(&buf, SHELL_SIZE, LOAD_SHELL);
}

int exec(char* file_name, int argc, char** argv) {
	char buf[EXE_SIZE];
    
    if(!read_file(&buf, EXE_SIZE, file_name)) {
	    return run_exe(&buf, EXE_SIZE, LOAD_EXE, argc, argv);
    }
    
    return 1;
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
      
    default:
		print_string("Unknown interrupt: ");
		print_hex_4(ax);
		print_string("!\r\n");
		break;
  }
}

void test() {

}

int init(){	
	clear_screen();
	
	makeInterrupt21();
	
	shell();
	//test();
	
	print_string("\n\nDone.");

	return 0;
}
