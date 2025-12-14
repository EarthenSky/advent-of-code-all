
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

#include "geblib/io.h"
#include "geblib/math.h"
#include "geblib/string.h"

struct red_tile {
    size_t x, y;
};

void parse_red_tiles(const char* file_path, size_t *red_tiles_size, struct red_tile **red_tiles) {
    struct file_info fi = get_file_contents(file_path);
    size_t num_red_tiles = count_occurences(fi.file_bytes, fi.num_bytes, '\n') + 1;

    *red_tiles_size = 0;
    *red_tiles = malloc(num_red_tiles * sizeof(struct red_tile));

    size_t ch_i = 0;
    while (ch_i < fi.num_bytes) {
        size_t x,y;
        size_t num_chars;
        int num_captures = sscanf(fi.file_bytes + ch_i, "%zu,%zu %zn", &x, &y, &num_chars);
        if (num_captures == EOF) {
            break;
        } else if (num_captures != 2) {
            exit(EXIT_FAILURE);
        }

        ch_i += num_chars;

        (*red_tiles)[*red_tiles_size] = (struct red_tile) { .x = x, .y = y };
        *red_tiles_size += 1;
    }

    free_file_info(fi);
}

int main() {
    size_t red_tiles_size;
    struct red_tile *red_tiles;
    parse_red_tiles("./2025/input_day9.txt", &red_tiles_size, &red_tiles);

    size_t largest_rectangle = 0;
    for (size_t i = 0; i < red_tiles_size; i++) {
        for (size_t j = i+1; j < red_tiles_size; j++) {
            size_t area =
                  (ll_abs((int64_t)red_tiles[i].x - (int64_t)red_tiles[j].x) + 1)
                * (ll_abs((int64_t)red_tiles[i].y - (int64_t)red_tiles[j].y) + 1);

            largest_rectangle = zmax(area, largest_rectangle);
        }
    }

    printf("(part1) largest_rectangle = %zu\n", largest_rectangle);

    free(red_tiles);
    return EXIT_SUCCESS;
}