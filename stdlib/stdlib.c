#include "stdlib.h"

void memset(int* buf, int val, int n) {    
    for ( ; n != 0; n--)
    	buf[n] = val;
}

void abort() {}