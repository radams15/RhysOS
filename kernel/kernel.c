#include "fat.h"
#include "tty.h"

void main();
void entry() {main();}

char msg[] = "Welcome!!!\r\n";

void main() {
	int err;

	err = init();

	if(err){
		print_string("\r\nError in kernel, halting!\r\n");
	}

	while(1){}
}


int handleInterrupt21(int ax, int bx, int cx, int dx) {
  switch(ax) {
    case 0:
		print_string((char *)bx);
		break;
	
    case 1:
		return readline(bx);
		break;
      
    default:
		print_string("Unknown interrupt: ");
		print_hex(ax);
		print_string("!\r\n");
  }
}

int init(){	
	clear_screen();
	
	makeInterrupt21();

	fat_test();

	return 0;
}
