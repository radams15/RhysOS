#ifndef MATH_H
#define MATH_H

int abs(int a);

int div(int dividend, int divisor);

int mod(int dividend, int divisor);

#define CEIL(x, y) ((x / y) + (x % y != 0))

#endif
