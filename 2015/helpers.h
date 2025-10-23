#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>

/// @brief you must free the file's contents
static char *get_file_contents(const char *filename) {
    FILE *f = fopen(filename, "r");

    fseek(f, 0, SEEK_END);
    size_t num_bytes = ftell(f);
    char *file_bytes = malloc(num_bytes+1);
    if (file_bytes == NULL) {
        printf("ERROR: malloc failed");
        return NULL;
    }

    fseek(f, 0, SEEK_SET);
    size_t num_bytes_read = fread(file_bytes, 1, num_bytes, f);
    if (num_bytes_read != num_bytes) {
        fprintf(stderr, "ERROR: fread() didn't read the correct number of bytes at %s:%d\n", __FILE__, __LINE__ - 2);
        fclose(f);
        free(file_bytes);
        return NULL;
    } else {
        fclose(f);
        file_bytes[num_bytes] = '\0';
        return file_bytes;
    }
}

/// @brief these don't exist in math.h !
static inline int min(int a, int b) { return a < b ? a : b; }
/// @brief these don't exist in math.h !
static inline int max(int a, int b) { return a > b ? a : b; }

#endif