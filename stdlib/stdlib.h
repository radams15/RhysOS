#ifndef STDLIB_H
#define STDLIB_H

typedef struct TimeDelta {
	int tick;
	int sec;
	int min;
	int hr;
} TimeDelta_t;

void memset(int* buf, int val, int n);

#endif
