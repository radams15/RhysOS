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


void handleInterrupt21(int ax, int bx, int cx, int dx) {
  switch(ax) {
    case 0 :
		print_string((char *)bx);
		break;
      
    default:
		print_string("Unknown interrupt: ");
		print_hex(ax);
		print_string("!\r\n");
  }
}

void putInMemory (int segment, int address, char character);

int init(){
	clear_screen();
	
	makeInterrupt21();

	fat_test();

	return 0;
}
