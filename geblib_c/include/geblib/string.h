#ifndef GEBLIB_STRING_H
#define GEBLIB_STRING_H

#include <stdlib.h>

size_t count_occurences(const char* str, size_t len, char ch) {
    size_t total = 0;
    for (size_t i = 0; i < len; i++)
        if (str[i] == ch)
            total += 1;
    return total;
}

#endif