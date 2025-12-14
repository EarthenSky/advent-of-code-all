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

    {
        bool *beams = malloc(2 * width * sizeof(*beams));
        bool *new_beams = beams + width;
        memset(beams, 0x00, 2 * width * sizeof(*beams));
        beams[start_x] = true;

        size_t *timelines = malloc(2 * width * sizeof(*timelines));
        size_t *new_timelines = timelines + width;
        memset(timelines, 0x00, width * sizeof(*timelines));
        timelines[start_x] = 1;

        size_t num_splits = 0;

        size_t ch_i = width + 1;
        while (ch_i <= fi.num_bytes) {
            memset(new_timelines, 0x00, width * sizeof(*new_timelines));

            size_t x;
            for (x = 0; x < width; x++) {
                assert(fi.file_bytes[ch_i + x] == '^' || fi.file_bytes[ch_i + x] == '.');

                if (beams[x]) {
                    if (fi.file_bytes[ch_i + x] == '^') {
                        new_beams[x-1] = true;
                        new_beams[x] = false;
                        new_beams[x+1] = true;

                        num_splits += 1;
                    } else if (fi.file_bytes[ch_i + x] == '.') {
                        new_beams[x] = true;
                    } 
                }

                if (timelines[x] != 0) {
                    if (fi.file_bytes[ch_i + x] == '^') {
                        new_timelines[x-1] += timelines[x];
                        new_timelines[x+1] += timelines[x];
                    } else if (fi.file_bytes[ch_i + x] == '.') {
                        new_timelines[x] += timelines[x];
                    }
                }
            }
            
            memcpy(beams, new_beams, width * sizeof(*beams));
            memcpy(timelines, new_timelines, width * sizeof(*timelines));

            ch_i += x;
            // skip the newline
            assert(fi.file_bytes[ch_i] == '\n');
            ch_i += 1;
        }

        free(beams);
        printf("(part1) num_splits = %zu\n", num_splits);

        size_t num_timelines = 0;
        for (size_t i = 0; i < width; i++)
            num_timelines += timelines[i];

        free(timelines);
        printf("(part2) num_timelines = %zu\n", num_timelines);
    }

    free_file_info(fi);
    return EXIT_SUCCESS;
}