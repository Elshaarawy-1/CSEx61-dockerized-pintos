#include<stdbool.h>
#include<stdint.h>

#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#define F (1 << 14)

typedef struct {
    int val;
} real;

real convert_int_to_real(int n);
int convert_real_to_int(real x, bool round); // if round: round to nearest else round toward 0
real add_real(real x, real y);
real sub_real(real x, real y); // y-x
real add_real_int(real x, int n);
real sub_real_int(real x, int n); // x-n
real mult_real(real x, real y);
real mult_real_int(real x, int n);
real div_real(real x, real y); // x/y
real div_real_int(real x, int n); // x/n

#endif