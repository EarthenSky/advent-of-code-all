#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>

/// @brief you must free the file's contents
char *get_file_contents(const char *filename) {
    FILE *f = fopen(filename, "r");

    fseek(f, 0, SEEK_END);
    size_t num_bytes = ftell(f);
    char *file_bytes = (char *) malloc(num_bytes+1);

    fseek(f, 0, SEEK_SET);
    size_t num_bytes_read = fread(file_bytes, 1, num_bytes, f);
    if (num_bytes_read != num_bytes) {
        printf("ERROR: didn't read the correct number of bytes");
        fclose(f);
        return NULL;
    } else {
        fclose(f);
        file_bytes[num_bytes] = '\0';
        return file_bytes;
    }
}

#endif