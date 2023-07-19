#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char** argv) {
	int fh;
	struct TimeDelta time;
	
	fh = open("/dev/time");
	
	read(fh, &time, sizeof(struct TimeDelta));
	
	close(fh);
	
	printf("%d:%d:%d:%d\n", time.hr, time.min, time.sec, time.tick);

	return 0;
}
