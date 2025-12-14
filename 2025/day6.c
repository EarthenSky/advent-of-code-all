#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ctype.h> // isdigit

#include "geblib/io.h"
#include "geblib/vector.h"

DEFINE_VECTOR(size_t)
DEFINE_VECTOR(char)

int main() {
    struct file_info fi = get_file_contents("./input_day6.txt");

    size_t height;
    {
        VECTOR_TYPE(size_t) numbers = VECTOR_CREATE(size_t, 1000);

        size_t ch_i = 0;
        while (isdigit(fi.file_bytes[ch_i])) {
            size_t x, num_chars;
            size_t num_captures = sscanf(fi.file_bytes + ch_i, "%zu %zn", &x, &num_chars);
            if (num_captures != 1) {
                fprintf(stderr, "ERROR: failed to capture next number.\n");
                exit(EXIT_FAILURE);
            }

            VECTOR_ADD(size_t, &numbers, x);

            ch_i += num_chars;
        }

        VECTOR_TYPE(char) operations = VECTOR_CREATE(char, 500);

        while (true) {
            char c;
            size_t num_chars;
            int num_captures = sscanf(fi.file_bytes + ch_i, "%c %zn", &c, &num_chars);
            if (num_captures == EOF) {
                break;
            } else if (num_captures != 1) {
                fprintf(stderr, "ERROR: failed to capture next number.\n");
                exit(EXIT_FAILURE);
            }

#ifdef CHECK_PRECONDITIONS
            assert(c == '+' || c == '*');
#endif

            VECTOR_ADD(char, &operations, c);

            ch_i += num_chars;
        }

#ifdef CHECK_PRECONDITIONS
        assert(numbers.size % operations.size == 0);
#endif

        size_t width = operations.size;
        height = numbers.size / width;

        size_t grand_total = 0;
        for (size_t i = 0; i < width; i++) {
            size_t local_total = operations.data[i] == '+' ? 0 : 1;
            for (size_t y = 0; y < height; y++) {
                if (operations.data[i] == '+')
                    local_total += numbers.data[y * width + i];
                else
                    local_total *= numbers.data[y * width + i];
            }

            grand_total += local_total;
        }

        printf("(part1) grand_total = %zu\n", grand_total);
    }

    {
        // TODO: assert file is LF, not CRLF
        
        size_t char_width = 0;
        while (fi.file_bytes[char_width] != '\n') {
            if (fi.file_bytes[char_width] == '\0') {
                fprintf(stderr, "ERROR: failed to find a newline.\n");
                exit(EXIT_FAILURE);
            }
            char_width += 1;
        }

        VECTOR_TYPE(size_t) numbers = VECTOR_CREATE(size_t, 50);

        size_t grand_total = 0;
        for (int64_t xi = char_width-1; xi >= 0; xi--) {
            // +1 includes the operation row too
            size_t current = 0;
            size_t digit_factor = 1;
            for (size_t y = 0; y < height; y++) {
                // work from bottom up (ie. least significant digit first)
                char current_c = fi.file_bytes[(height - 1 - y) * (char_width+1) + xi];
                // ignore spaces
                if (current_c != ' ') {
                    current += (current_c - '0') * digit_factor;
                    digit_factor *= 10;
                }
            }

            // TODO: assert that 0 cannot show up anywhere in the file
            if (current == 0)
                continue;

            // check what operation we have
            char op_c = fi.file_bytes[height * (char_width+1) + xi];

            VECTOR_ADD(size_t, &numbers, current);

            if (op_c == '+') {
                size_t og_size = numbers.size;
                for (size_t i = 0; i < og_size; i++) {
                    size_t popped = VECTOR_POP(size_t, &numbers);
                    grand_total += popped;
                }
            } else if (op_c == '*') {
                size_t local_total = 1;
                size_t og_size = numbers.size;
                for (size_t i = 0; i < og_size; i++)
                    local_total *= VECTOR_POP(size_t, &numbers);
                grand_total += local_total;
            } else if (op_c != ' ') {
                fprintf(stderr, "ERROR: unreachable\n");
                exit(EXIT_FAILURE);
            }
        }

        // < 10362326419132
        printf("(part2) grand_total = %zu\n", grand_total);
    }

    free_file_info(fi);
    return EXIT_SUCCESS;
}