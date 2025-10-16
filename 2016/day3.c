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

bool is_valid_triangle(uint64_t a, uint64_t b, uint64_t c) {
    // If the sum of smallest sides is larger than the largest sides, then
    // all sums of two sides are larger than any side.
    uint64_t largest_side = max(max(a, b), c);
    uint64_t other_sides;
    if (largest_side == a) {
        other_sides = b + c;
    } else if (largest_side == b) {
        other_sides = a + c;
    } else if (largest_side == c) {
        other_sides = a + b;
    }

    return other_sides > largest_side;
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

        if (is_valid_triangle(a, b, c))
            num_valid_triangles += 1;
    }

    printf("num_valid_triangles = %u\n", num_valid_triangles);
}

void part2(const char *file_contents, size_t file_size) {
    printf("part2:\n");

    // make a copy for strtok to mangle
    // char *file_contents_copy = (char *) malloc(file_size+1);
    // memcpy(file_contents_copy, file_contents, file_size+1);

    uint32_t num_valid_triangles = 0;
    size_t file_i = 0;
    while (true) {
        uint64_t v[9];
        size_t tokens_parsed = 0;
        int num_matches = sscanf(
            file_contents + file_i,
            "%llu %llu %llu %llu %llu %llu %llu %llu %llu %n",
            &v[0], &v[1], &v[2],
            &v[3], &v[4], &v[5],
            &v[6], &v[7], &v[8],
            &tokens_parsed
        );
        if (num_matches == 0 || num_matches == EOF) {
            break;
        } else if (num_matches != 9) {
            perror("ERROR: the file should have a number of integers divisible by 9");
            return;
        }

        file_i += tokens_parsed;

        if (is_valid_triangle(v[0], v[3], v[6]))
            num_valid_triangles += 1;
        if (is_valid_triangle(v[1], v[4], v[7]))
            num_valid_triangles += 1;
        if (is_valid_triangle(v[2], v[5], v[8]))
            num_valid_triangles += 1;
    }

    printf("num_valid_triangles = %u\n", num_valid_triangles);
}