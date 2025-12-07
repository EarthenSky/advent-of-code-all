#ifndef GEBLIB_MATH_H
#define GEBLIB_MATH_H

#include <stdlib.h>

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

size_t z_pow10(size_t x) {
    size_t result = 1;
    for (size_t i = 0; i < x; i++)
        result *= 10;
    return result;
}

#endif