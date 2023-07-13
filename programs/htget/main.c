#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int comfd;

#define WRITE(data, len) write(comfd, data, len)

int connect_com() {
    char buf[512];
	comfd = open("/dev/com1");
	write(comfd, "AT", 2);
	
	read(comfd, buf, 2);
	if(strncmp(buf, "AT", 2) != 0) {
	    return 1;
	}
	
	return 0;
}

int get(char* domain, char* page) {
    WRITE("ATDT ", 5);
    WRITE(domain, strlen(domain));
    WRITE("\r\n", 2);
    WRITE("GET ", 4);
    WRITE(page, strlen(page));
    WRITE(" HTTP/1.1\r\n", 11);
}

int main(int argc, char** argv) {
    if(connect_com()) {
        fprintf(stderr, "Failed to open COM1!\n");
        return 1;
    }
    
    get("google.com:80", "/");
	
	close(comfd);
	
	return 0;
}
