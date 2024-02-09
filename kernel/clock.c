#include "clock.h"

#include "tty.h"

TimeDelta_t counter;

tick_callback_t callbacks[32];
tick_callback_t* callback_ptr = &callbacks;

int add_tick_callback(tick_callback_t callback) {
    *callback_ptr = callback;
    callback_ptr++;

    return 0;
}

int call_tick_handlers() {
    for(tick_callback_t* c=&callbacks ; c != callback_ptr ; c++) {
        (*c)();
    }

    return 0;
}

int time(struct TimeDelta* buf) {
    memcpy(buf, &counter, sizeof(struct TimeDelta));

    return 0;
}

void tick() {
    counter.tick = counter.tick + 1;

    if (counter.tick >= 18) {
        counter.tick = 0;
        counter.sec++;
        call_tick_handlers(); 
    }

    if (counter.sec >= 60) {
        counter.sec = 0;
        counter.min++;
    }

    if (counter.min >= 60) {
        counter.min = 0;
        counter.hr++;
    }

}

int bcdToDecimal(int bcdValue) {
    return ((bcdValue / 16) * 10) + (bcdValue % 16);
}

int rtc_init() {
    unsigned int cmos_time[4] = {0, 0, 0, 0};

    if (clock_time(&cmos_time)) {
        return 1;
    }

    counter.tick = 0;
    counter.sec = bcdToDecimal(cmos_time[2]);
    counter.min = bcdToDecimal(cmos_time[1]);
    counter.hr = bcdToDecimal(cmos_time[0]);

    return 0;
}
