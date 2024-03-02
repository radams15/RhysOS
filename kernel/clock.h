#ifndef CLOCK_H
#define CLOCK_H

typedef struct TimeDelta {
    int tick;
    int sec;
    int min;
    int hr;
} TimeDelta_t;


int time(struct TimeDelta* buf);
int rtc_init();

unsigned int clock_ticks(unsigned int* buf);
int clock_time(unsigned int* buf);

void clock_tick();
typedef void (*tick_callback_t)(void);

int add_tick_callback(tick_callback_t callback);

#endif
