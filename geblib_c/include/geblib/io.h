#ifndef GEBLIB_IO_H
#define GEBLIB_IO_H

#include <stdlib.h>

struct file_info {
    char *file_bytes;
    // not including the null terminator
    size_t num_bytes;
};

/// @brief you must free fi.file_bytes, unless num_bytes is 0
struct file_info get_file_contents(const char *filename);

void free_file_info(struct file_info fi);

#endif