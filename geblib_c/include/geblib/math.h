#ifndef GEBLIB_MATH_H
#define GEBLIB_MATH_H

#include <stdint.h>
#include <stdlib.h>

// static inline is for TU-local linkage
static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

static inline size_t zmin(size_t a, size_t b) { return a < b ? a : b; }
static inline size_t zmax(size_t a, size_t b) { return a > b ? a : b; }

static inline size_t z_pow10(size_t x) {
    size_t result = 1;
    for (size_t i = 0; i < x; i++)
        result *= 10;
    return result;
}

static inline int64_t ll_abs(int64_t x) {
    return x >= 0 ? x : -x;
}

#endif