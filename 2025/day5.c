#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <assert.h>

#include "geblib/io.h"
#include "geblib/math.h" // zmin, zmax
#include "geblib/vector.h"

#define CHECK_PRECONDITIONS

typedef struct range {
    size_t start;
    size_t end;
} range_t;

DEFINE_VECTOR(range_t)

bool range_contains(range_t range, size_t x) {
    return x >= range.start && x <= range.end;
}

bool ranges_overlap(range_t range_a, range_t range_b) {
    return (
        range_contains(range_a, range_b.start)
        || (range_b.start < range_a.start && range_b.end >= range_a.start)
    );
}

range_t enclosing_range(range_t range_a, range_t range_b) {
    //printf("%zu,%zu u %zu,%zu = %zu,%zu\n",
    //    range_a.start, range_a.end,
    //    range_b.start, range_b.end,
    //    zmin(range_a.start, range_b.start), zmax(range_a.end, range_b.end));
    return (range_t) {
        // I wish the C stdlib had basic generics
        // TODO: What if I made my own stdlib? What even is the spec? I'll make my own.
        .start = zmin(range_a.start, range_b.start),
        .end = zmax(range_a.end, range_b.end),
    };
}

int main() {
    struct file_info fi = get_file_contents("./input_day5.txt");

    VECTOR_TYPE(range_t) ranges = VECTOR_CREATE(range_t, 200);

    size_t ch_i = 0;
    while (true) {
        range_t new_range;
        size_t num_chars_before, num_chars_after;
        int num_captures = sscanf(
            fi.file_bytes + ch_i,
            "%zu-%zu%zn %zn",
            &new_range.start, &new_range.end, &num_chars_before, &num_chars_after
        );
        if (num_captures != 2) {
            fprintf(stderr, "ERROR: failed to get range\n");
            exit(EXIT_FAILURE);
        }

#ifdef CHECK_PRECONDITIONS
        assert(new_range.end >= new_range.start);
#endif

        ch_i += num_chars_after;

        // if range overlaps with any other, combine them
        bool found_overlap = false;
        for (size_t i = 0; i < ranges.size; i++)
            if (ranges_overlap(ranges.data[i], new_range)) {
                ranges.data[i] = enclosing_range(ranges.data[i], new_range);
                found_overlap = true;
                break;
            }

        if (!found_overlap)
            VECTOR_ADD(range_t, &ranges, new_range);

        size_t trailing_whitespace = num_chars_after - num_chars_before;
        if (trailing_whitespace > 1)
            break;
    }

    {
        // TODO: to improve performance, sort ingredients first, then only traverse $ranges once

        size_t num_fresh_ingredients = 0;

        while(true) {
            size_t ingredient_id;
            size_t num_chars;
            int num_captures = sscanf(fi.file_bytes + ch_i, "%zu %zn", &ingredient_id, &num_chars);
            if (num_captures == EOF) {
                break;
            } else if (num_captures != 1) {
                fprintf(stderr, "ERROR: failed to get ingredient id\n");
                exit(EXIT_FAILURE);
            }

            ch_i += num_chars;

            for (size_t i = 0; i < ranges.size; i++)
                if (range_contains(ranges.data[i], ingredient_id)) {
                    num_fresh_ingredients += 1;
                    break;
                }
        }

        printf("(part1) num_fresh_ingredients = %zu\n", num_fresh_ingredients);
    }

    size_t total_num_fresh_ingredients = 0;

    // a range either overlaps with another range in the list, or overlaps with none
    size_t original_num_ranges = ranges.size;
    for (size_t range_i = 0; range_i < original_num_ranges; range_i++) {
#ifdef CHECK_PRECONDITIONS
        assert(ranges.size > 0);
#endif

        range_t end_range = VECTOR_POP(range_t, &ranges);

#ifdef CHECK_PRECONDITIONS
        assert(end_range.end >= end_range.start);
#endif

        bool found_overlap = false;
        for (size_t i = 0; i < ranges.size; i++)
            if (ranges_overlap(ranges.data[i], end_range)) {
                ranges.data[i] = enclosing_range(ranges.data[i], end_range);
                found_overlap = true;
                break;
            }

        if (!found_overlap) {
            size_t range_size = (end_range.end - end_range.start) + 1;
            printf("range_size = %zu for (%zu,%zu)\n", range_size, end_range.start, end_range.end);
            total_num_fresh_ingredients += range_size;
        }
    }

    // > 144658707863913
    // > 147839499615650
    // < 365641407219074
    // ! 365641407219175

    printf("(part2) total_num_fresh_ingredients = %zu\n", total_num_fresh_ingredients);

    return EXIT_SUCCESS;
}