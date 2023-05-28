#include "clock.h"

#include "tty.h"

extern void make_rtc_interrupt();

void tick() {
	print_string("TICK\n");
}

int rtc_init() {
	make_rtc_interrupt();
	
	return 0;
}
