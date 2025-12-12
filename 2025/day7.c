#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <assert.h>

#include "geblib/io.h"

int main() {
    struct file_info fi = get_file_contents("./2025/input_day7.txt");

    int start_x = -1;
    size_t width = 0;
    while (fi.file_bytes[width] != '\n') {
        if (fi.file_bytes[width] == 'S')
            start_x = width;
        width += 1;
    }
    assert(start_x >= 0);

    bool *beams = malloc(2 * width * sizeof(*beams));
    bool *new_beams = beams + width;
    memset(beams, 0x00, 2 * width * sizeof(*beams));
    beams[start_x] = true;

    size_t num_splits = 0;

    size_t ch_i = width + 1;
    while (ch_i <= fi.num_bytes) {
        size_t x;
        for (x = 0; x < width; x++) {
            if (!beams[x])
                continue;

            if (fi.file_bytes[ch_i + x] == '^') {
                new_beams[x-1] = true;
                new_beams[x] = false;
                new_beams[x+1] = true;

                num_splits += 1;
            } else if (fi.file_bytes[ch_i + x] == '.') {
                new_beams[x] = true;
            } else {
                fprintf(stderr, "ERROR: unexpected character.\n");
                exit(EXIT_FAILURE);
            }
        }

        memcpy(beams, new_beams, width * sizeof(*beams));

        ch_i += x;
        // skip the newline
        assert(fi.file_bytes[ch_i] == '\n');
        ch_i += 1;
    }

    free(beams);

    printf("(part1) num_splits = %zu\n", num_splits);

    return EXIT_SUCCESS;
}