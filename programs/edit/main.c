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
	return interrupt(0x10, (0x0B<<8), (0x00<<8) + bg, 0, 0);
}

int graphics_set_palette(char palette) {
	return interrupt(0x10, (0x0B<<8), (0x01<<8) + palette, 0, 0);
}

int graphics_putc(int x, int y, int colour) {
	return interrupt(0x10, (0x0C<<8) + colour, 0, x, y);
}

void render(int fd) {
    seek(fd, 0);
}

int topbar() {
	int x, y;
	for(x=0 ; x<80 ; x++) // Draw top bar
			for(y=0 ; y<1 ; y++)
				graphics_putc(x, y, 1);
}

int mainloop(char* fname) {
    int fd;
    
    fd = open(fname);
    if(fd == -1)
        return 1;
	
	graphics_set_palette(2);
	graphics_set_bg(1);
	
	topbar();
	
    		
    return 0;
}

int main(int argc, char** argv) {
    int before_mode;
    int err;
    
    before_mode = get_graphics_mode();
    
    set_graphics_mode(GRAPHICS_COLOUR_80x25);
    
    err = mainloop("/test.bat");
    
    getch();
    
    set_graphics_mode(before_mode);
    
    if(err)
        fprintf(stderr, "Failed to open file!\n");
	
	return 0;
}
