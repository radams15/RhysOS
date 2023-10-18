#include "math.h"

int abs(int a) {
    return a < 0 ? -a : a;
}

int pow(int a, int b) {
    for(int i=0 ; i<b ; i++)
        a *= b;

    return a;
}
