#include "threads/fixed_point.h"



real convert_int_to_real(int n){
    real r;
    r.val = n * F;
    return r;
}

int convert_real_to_int(real x, bool round){
    if(round){
        if(x.val >= 0){
            return (x.val + F/2) / F;
        }else{
            return (x.val - F/2) / F;
        }
    }else{
        return x.val / F;
    }
}

real add_real(real x, real y){
    real r;
    r.val = x.val + y.val;
    return r;
}

real sub_real(real x, real y){
    real r;
    r.val = x.val - y.val;
    return r;
}

real add_real_int(real x, int n){
    real r;
    r.val = x.val + n * F;
    return r;
}

real sub_real_int(real x, int n){
    real r;
    r.val = x.val - n * F;
    return r;
}

real mult_real(real x, real y){
    real r;
    r.val = ((int64_t)x.val) * y.val / F;
    return r;
}

real mult_real_int(real x, int n){
    real r;
    r.val = x.val * n;
    return r;
}

real div_real(real x, real y){
    real r;
    r.val = ((int64_t)x.val) * F / y.val;
    return r;
}

real div_real_int(real x, int n){
    real r;
    r.val = x.val / n;
    return r;
}
