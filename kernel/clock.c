#include "clock.h"

#include "tty.h"

TimeDelta_t counter;

extern void make_rtc_interrupt();

int time(struct TimeDelta* buf) {
	memcpy(buf, &counter, sizeof(struct TimeDelta));
	
	return 0;
}

void tick() {
	counter.tick = counter.tick + 1;
	
	if(counter.tick >= 18) {
		counter.tick = 0;
		counter.sec++;
	}
	
	if(counter.sec >= 60) {
		counter.sec = 0;
		counter.min++;
	}

	if(counter.min >= 60) {
		counter.min = 0;
		counter.hr++;
	}
}

int rtc_init() {
	counter.tick = 0;
	counter.sec = 0;
	counter.min = 0;
	counter.hr = 0;
	
	make_rtc_interrupt();
	
	return 0;
}
