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

struct file_info {
    char *file_bytes;
    // not including the null terminator
    size_t num_bytes;
};

/// @brief you must free fi.file_bytes, unless num_bytes is 0
static struct file_info get_file_contents_and_size(const char *filename) {
    struct file_info fi = { .file_bytes = 0, .num_bytes = 0 };
    
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("fopen() ERROR");
        return fi;
    }

    if (fseek(f, 0, SEEK_END)) {
        fprintf(stderr, "ERROR: fseek() failed at %s:%d\n", __FILE__, __LINE__ - 1);
        fclose(f);
        return fi;
    }
    // TODO: use errno
    fi.num_bytes = ftell(f);
    fi.file_bytes = malloc(fi.num_bytes+1);
    if (fi.file_bytes == NULL) {
        fprintf(stderr, "ERROR: malloc() failed at %s:%d\n", __FILE__, __LINE__ - 2);
        fclose(f);
        fi.num_bytes = 0;
        return fi;
    }

    if (fseek(f, 0, SEEK_SET)) {
        fprintf(stderr, "ERROR: fseek() failed at %s:%d\n", __FILE__, __LINE__ - 2);
        fclose(f);
        free(fi.file_bytes);
        fi.file_bytes = NULL;
        fi.num_bytes = 0;
        return fi;
    }
    size_t num_bytes_read = fread(fi.file_bytes, 1, fi.num_bytes, f);
    if (num_bytes_read != fi.num_bytes) {
        fprintf(stderr, "ERROR: fread() didn't read the correct number of bytes at %s:%d\n", __FILE__, __LINE__ - 2);
        fclose(f);
        free(fi.file_bytes);
        fi.file_bytes = NULL;
        fi.num_bytes = 0;
        return fi;
    } else {
        fclose(f);
        fi.file_bytes[fi.num_bytes] = '\0';
        return fi;
    }
}

void free_file_info(struct file_info fi) {
    free(fi.file_bytes);
}

/// @brief these don't exist in math.h !
static inline int min(int a, int b) { return a < b ? a : b; }
/// @brief these don't exist in math.h !
static inline int max(int a, int b) { return a > b ? a : b; }

#endif