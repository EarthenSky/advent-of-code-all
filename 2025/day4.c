#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "../geblib_c/io.h"

const char ROLL = '@';
const char NONE = '.';

// TODO: the code is better if map is padded
// TODO: benchmark it
// TODO: how to measure cycles?
// TODO: would be a fun short blog post!
size_t num_adjacent_rolls(const char* map, size_t height, size_t width, size_t x, size_t y) {
    size_t num = 0;

    if (x != 0) {
        num += (map[(width+1) * y + (x-1)] == ROLL) ? 1 : 0;
        if (y != 0)
            num += (map[(width+1) * (y-1) + (x-1)] == ROLL) ? 1 : 0;
        if (y != height-1)
            num += (map[(width+1) * (y+1) + (x-1)] == ROLL) ? 1 : 0;
    }

    if (x != width-1) {
        num += (map[(width+1) * y + (x+1)] == ROLL) ? 1 : 0;
        if (y != 0)
            num += (map[(width+1) * (y-1) + (x+1)] == ROLL) ? 1 : 0;
        if (y != height-1)
            num += (map[(width+1) * (y+1) + (x+1)] == ROLL) ? 1 : 0;
    }

    if (y != 0)
        num += (map[(width+1) * (y-1) + x] == ROLL) ? 1 : 0;
    if (y != height-1)
        num += (map[(width+1) * (y+1) + x] == ROLL) ? 1 : 0;

    return num;
}

size_t count_accessible_rolls(const char* map, size_t height, size_t width) {
#ifdef CHECK_PRECONDITIONS
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            assert(
                // width does not include the newline at the end of every row
                map[(width+1) * y + x] == ROLL
                || map[(width+1) * y + x] == NONE 
            )
#endif

    size_t num_accessible = 0;

    for (size_t y = 0; y < height; y++)
        for (size_t x = 0; x < width; x++)
            if (
                map[(width+1) * y + x] == ROLL
                && num_adjacent_rolls(map, height, width, x, y) < 4
            )
                num_accessible += 1;

    return num_accessible;
}

size_t remove_accessible_rolls(char* map, size_t height, size_t width) {
#ifdef CHECK_PRECONDITIONS
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            assert(
                // width does not include the newline at the end of every row
                map[(width+1) * y + x] == ROLL
                || map[(width+1) * y + x] == NONE 
            )
#endif

    size_t num_removed = 0;

    for (size_t y = 0; y < height; y++)
        for (size_t x = 0; x < width; x++)
            if (
                map[(width+1) * y + x] == ROLL
                && num_adjacent_rolls(map, height, width, x, y) < 4
            ) {
                num_removed += 1;
                map[(width+1) * y + x] = NONE;
            }

    return num_removed;
}

int main() {
    struct file_info fi = get_file_contents("./input_day4.txt");

    size_t width;
    int num_captures = sscanf(fi.file_bytes, "%*s%zn ", &width);
    if (num_captures < 0) {
        fprintf(stderr, "ERROR: failed to get file width\n");
        exit(EXIT_FAILURE);
    }

    // NOTE: we also assume the file does not end in a newline
    size_t height = (fi.num_bytes+1) / (width + 1);

    // < 2223
    // < 1430
    printf("(part1) num_accessible_rolls = %zu\n", count_accessible_rolls(fi.file_bytes, width, height));

    size_t total_rolls_removed = 0;
    while (true) {
        size_t num_removed = remove_accessible_rolls(fi.file_bytes, width, height);
        if (num_removed == 0)
            break;

        total_rolls_removed += num_removed;
    }

    printf("(part2) total_rolls_removed = %zu\n", total_rolls_removed);

    return EXIT_SUCCESS;
}