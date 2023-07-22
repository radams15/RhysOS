#include <stdio.h>
#include <string.h>
#include <syscall.h>

int get_graphics_mode() {
    char buf[2];
    int fh;
    int mode;
    
    fh = open("/dev/graphmode");
    
    read(fh, &buf, sizeof(buf));
    mode = chars2int(buf);
    
    close(fh);
    
    return mode;
}

void set_graphics_mode(int mode) {
    char buf[2];
    int fh;
    
    int2chars(mode, buf);
    
    fh = open("/dev/graphmode");
    
    write(fh, &buf, sizeof(buf));
    
    close(fh);
}


int graphics_set_bg(char bg) {
	return interrupt_10((0x0B<<8), (0x00<<8) + bg, 0, 0);
}

int graphics_set_palette(char palette) {
	return interrupt_10((0x0B<<8), (0x01<<8) + palette, 0, 0);
}

int graphics_putc(int x, int y, int colour) {
	return interrupt_10((0x0C<<8) + colour, 0, x, y);
}

void mainloop() {
    int x, y, colour;
    colour = 2;
	
	graphics_set_palette(2);
	graphics_set_bg(1);
	
	colour = 1;
	x = 30;
	y = 100;
	while(x <= 60) {	
		graphics_putc(x, y, colour);
		x++;
		y++;
	}
}

int main(int argc, char** argv) {
    int before_mode;
    
    before_mode = get_graphics_mode();
    
    printf("Mode: %d\n", before_mode);
    
    set_graphics_mode(GRAPHICS_CGA_320x200);
    
    mainloop();

    getch();
    
    set_graphics_mode(before_mode);
	
	return 0;
}
