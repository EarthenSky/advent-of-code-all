#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// #include <math.h>

#include "geblib/io.h"
#include "geblib/hashset.h"
#include "geblib/math.h"

DEFINE_HASHSET(size_t)

struct range_t {
    size_t start;
    size_t end;
};

size_t num_digits(size_t x) {
    char buffer[64];
    int num_chars = sprintf(buffer, "%zu", x);
    if (num_chars < 0) {
        fprintf(stderr, "ERROR: failed to write integer to buffer in num_digits()\n");
        exit(EXIT_FAILURE);
    }
    return strlen(buffer);
}

size_t sum_invalid_ids_repeated_twice(struct range_t range) {
    size_t sum = 0;
    size_t current = range.start;
    while (current <= range.end) {
        char buffer[64];
        int num_chars = sprintf(buffer, "%zu", current);
        if (num_chars < 0) {
            fprintf(stderr, "ERROR: failed to write integer to buffer\n");
            exit(EXIT_FAILURE);
        }

        size_t length = strlen(buffer);

        if (length % 2 == 1) {
            current = z_pow10(length);
        } else {
            size_t half_zeros = z_pow10(length/2);

            size_t upper = current / half_zeros;
            size_t lower = current % half_zeros;

            if (upper == lower) {
                sum += current;
                current += half_zeros - lower;
            } else if (upper < lower) {
                current += half_zeros - lower;
            } else if (upper > lower) {
                current = current - lower + upper;
            }
        }
    }
    return sum;
}

size_t sum_invalid_ids_repeated(HASHSET_TYPE(size_t) visited, struct range_t range, size_t n_times) {
    if (n_times < 2) {
        fprintf(stderr, "ERROR: unexpected n_times < 2\n");
        exit(EXIT_FAILURE);
    }

    size_t sum = 0;
    size_t current = range.start;
    while (current <= range.end) {
        char buffer[64];
        int num_chars = sprintf(buffer, "%zu", current);
        if (num_chars < 0) {
            fprintf(stderr, "ERROR: failed to write integer to buffer\n");
            exit(EXIT_FAILURE);
        }

        size_t length = strlen(buffer);

        if (length % n_times != 0) {
            // increase length by 1
            current = z_pow10(length);
        } else {
            size_t chunk_size = length / n_times;
            size_t chunk_zeros = z_pow10(chunk_size);

            size_t leading_chunk = current / z_pow10(length - chunk_size);

            bool success = true;
            for (size_t i = n_times-1; i > 0; i--) {
                size_t current_chunk_zeros = z_pow10(length - i * chunk_size);
                size_t current_chunk = (current % current_chunk_zeros) / (current_chunk_zeros / chunk_zeros);

                if (current_chunk < leading_chunk) {
                    //current = current - lower * z_pow10(chunk_size) + lower_zeros;
                    current += (leading_chunk - current_chunk) * (current_chunk_zeros / chunk_zeros);
                    success = false;
                    break;
                } else if (leading_chunk < current_chunk) {
                    current += (chunk_zeros - current_chunk) * (current_chunk_zeros / chunk_zeros);
                    success = false;
                    break;
                }
            }

            if (success) {
                if (!HASHSET_CONTAINS(size_t, &visited, &current)) {
                    sum += current;
                    HASHSET_INSERT(size_t, &visited, &current);
                } 
                current += 1;
            }
        }
    }

    return sum;
}

size_t size_t_hash_function(const size_t *x) {
    return *x;
}
bool size_t_equality_function(const size_t *x, const size_t *y) {
    return *x == *y;
}

int main() {
    size_t double_repeat_sum = 0;
    size_t more_than_double_repeat_sum = 0;

    HASHSET_TYPE(size_t) visited = HASHSET_CREATE(size_t, 1000, size_t_hash_function, size_t_equality_function);

    struct file_info fi = get_file_contents("./input_day2.txt");
    size_t ch_i = 0;
    while (ch_i < fi.num_bytes) {
        size_t a, b;
        size_t num_chars = 0;

        int num_matches = sscanf(fi.file_bytes + ch_i, "%zu - %zu%zn", &a, &b, &num_chars);

        // + 1 skips the trailing comma
        ch_i += num_chars + 1;

        if (num_matches == EOF) {
            // in case of trailing whitespace
            break;
        } else if (num_matches != 2) {
            fprintf(stderr, "ERROR: failed to parse range.\n");
            exit(EXIT_FAILURE);
        }

        double_repeat_sum += sum_invalid_ids_repeated_twice((struct range_t) {
            .start = a, .end = b
        });
    
        for (size_t n = 2; n <= num_digits(b); n++)
            more_than_double_repeat_sum += sum_invalid_ids_repeated(visited, (struct range_t) {
                .start = a, .end = b
            }, n);
    }

    printf("(part1) total_sum = %zu\n", double_repeat_sum);
    // < 324136034506
    // > 54370990224
    // = 54486209192
    printf("(part2) total_sum = %zu\n", more_than_double_repeat_sum);
    return 1;
}