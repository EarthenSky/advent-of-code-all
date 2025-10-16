#include <assert.h>
#include <stdint.h> // ?
#include <stdio.h>  // sscanf() ?
#include <stdlib.h> // malloc(), free()
#include <stdbool.h> // true / false
#include <string.h> // strlen()

#include <math.h> // max()

#include <sys/types.h> // ssize_t

#include "../2015/helpers.h"

void part1(const char *file_contents, size_t file_size);
void part2(const char *file_contents, size_t file_size);

int main() {
    char *file_contents = get_file_contents("input_day3.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

void part1(const char *file_contents, size_t file_size) {
    printf("part 1:\n");

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    uint32_t num_valid_triangles = 0;
    for (char *line = strtok(file_contents_copy, "\n"); line != NULL; line = strtok(NULL, "\n")) {
        uint64_t a, b, c;
        int num_matches = sscanf(line, "%llu %llu %llu", &a, &b, &c);
        assert(num_matches == 3);

        // order our 3 numbers by size
        uint64_t largest_side = max(max(a, b), c);
        uint64_t middle, smallest;
        if (largest_side == a && b > c) {
            middle = b;
            smallest = c;
        } else if (largest_side == a && c > b) {
            middle = c;
            smallest = b;
        } else if (largest_side == b && a > c) {
            middle = a;
            smallest = c;
        } else if (largest_side == b && c > a) {
            middle = c;
            smallest = a;
        } else if (largest_side == c && a > b) {
            middle = a;
            smallest = b;
        } else if (largest_side == c && b > a) {
            middle = b;
            smallest = a;
        }

        if (middle + smallest > largest_side)
            num_valid_triangles += 1;
    }

    printf("num_valid_triangles = %u\n", num_valid_triangles);
}

void part2(const char *file_contents, size_t file_size) {
    printf("part2:\n");

}