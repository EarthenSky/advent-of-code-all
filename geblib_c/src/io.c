#include <stdio.h>

// TODO: can cmake figure out where my include directory is? It should... right?
#include "geblib/io.h"

struct file_info get_file_contents(const char *filename) {
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
    // TODO: use errno to hit the ftell failure condition
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