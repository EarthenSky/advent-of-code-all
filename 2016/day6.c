#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <iso646.h>

#include "../2015/helpers.h"

#define NUM_LETTERS (26)

size_t arg_max_histogram(uint32_t histogram[NUM_LETTERS]) {
    uint32_t max_value = histogram[0];
    size_t max_value_i = 0;
    for (size_t i = 1; i < NUM_LETTERS; i++) {
        if (histogram[i] > max_value) {
            max_value = histogram[i];
            max_value_i = i;
        }
    }

    return max_value_i;
}

size_t arg_min_histogram(uint32_t histogram[NUM_LETTERS]) {
    uint32_t min_value = histogram[0];
    size_t min_value_i = 0;
    for (size_t i = 1; i < NUM_LETTERS; i++) {
        if (histogram[i] < min_value) {
            min_value = histogram[i];
            min_value_i = i;
        }
    }

    return min_value_i;
}

void fold_lines_by(
    char *message_out,
    size_t message_length,
    const char *file_bytes,
    size_t num_lines,
    size_t (*extract_from_histogram) (uint32_t[NUM_LETTERS])
) {
    for (size_t ch_i = 0; ch_i < message_length; ch_i++) {
        uint32_t histogram[NUM_LETTERS] = { 0 };

        // fill histogram
        for (size_t line_i = 0; line_i < num_lines; line_i++) {
            char ch = file_bytes[line_i * (message_length+1) + ch_i];
            if (ch >= 'a' and ch <= 'z') {
                histogram[ch - 'a'] += 1;
            } else {
                fprintf(
                    stderr,
                    "ERROR: Got unexpectected char `%c` in word at %s:%d\n",
                    ch, __FILE__, __LINE__ - 4
                );
                exit(EXIT_FAILURE);
            }
        }

        message_out[ch_i] = 'a' + extract_from_histogram(histogram);
    }
}

void part1(const char *file_bytes, size_t num_bytes) {
    // we expect all words are the same length, every newline is a single char,
    // and that there is no final newline.
    size_t message_length;
    if (sscanf(file_bytes, "%*s%zn", &message_length) != 0) {
        fprintf(stderr, "ERROR: sscanf() failed to match string at %s:%d\n", __FILE__, __LINE__ - 1);
        exit(EXIT_FAILURE);
    }
    size_t num_lines = (num_bytes + 1) / (message_length + 1);

    char *message = malloc((message_length+1) * sizeof *message);
    if (message == NULL) {
        fprintf(stderr, "ERROR: malloc() failed at %s:%d\n", __FILE__, __LINE__ - 2);
        exit(EXIT_FAILURE);
    }
    message[message_length] = '\0';

    fold_lines_by(
        message, message_length,
        file_bytes, num_lines,
        arg_max_histogram
    );

    printf("error corrected message = %s\n", message);
    free(message);
}

void part2(const char *file_bytes, size_t num_bytes) {
    // we expect all words are the same length, every newline is a single char,
    // and that there is no final newline.
    size_t message_length;
    if (sscanf(file_bytes, "%*s%zn", &message_length) != 0) {
        fprintf(stderr, "ERROR: sscanf() failed to match string at %s:%d\n", __FILE__, __LINE__ - 1);
        exit(EXIT_FAILURE);
    }
    size_t num_lines = (num_bytes + 1) / (message_length + 1);

    char *message = malloc((message_length+1) * sizeof *message);
    if (message == NULL) {
        fprintf(stderr, "ERROR: malloc() failed at %s:%d\n", __FILE__, __LINE__ - 2);
        exit(EXIT_FAILURE);
    }
    message[message_length] = '\0';

    fold_lines_by(
        message, message_length,
        file_bytes, num_lines,
        arg_min_histogram
    );

    printf("error corrected message = %s\n", message);
    free(message);
}

int main() {
    struct file_info fi = get_file_contents_and_size("./input_day6.txt");
    part1(fi.file_bytes, fi.num_bytes);
    part2(fi.file_bytes, fi.num_bytes);
    free(fi.file_bytes);
    return EXIT_SUCCESS;
}