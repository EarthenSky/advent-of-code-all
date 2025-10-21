#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../2015/helpers.h"

#define SCREEN_HEIGHT (6)
#define SCREEN_WIDTH (50)
#define SCREEN_NUM_PIXELS (SCREEN_HEIGHT * SCREEN_WIDTH)

void rect(bool screen[SCREEN_NUM_PIXELS], uint8_t a, uint8_t b) {
    // Q: is memset faster than a loop?
    for (size_t y = 0; y < b; y++)
        memset(screen + y * SCREEN_WIDTH, true, a * sizeof(bool));
}

void rotate_row(bool screen[SCREEN_NUM_PIXELS], uint8_t row_i, uint8_t right_shift_amount) {
    bool *row_start = screen + row_i * SCREEN_WIDTH;
    bool row_copy[SCREEN_WIDTH];
    memcpy(row_copy, row_start, SCREEN_WIDTH * sizeof(bool));

    // size of the shifted segment
    size_t shifted_size = SCREEN_WIDTH - right_shift_amount;
    memcpy(row_start, row_copy + shifted_size, right_shift_amount * sizeof(bool));
    memcpy(row_start + right_shift_amount, row_copy, shifted_size * sizeof(bool));
}

void rotate_column(bool screen[SCREEN_NUM_PIXELS], uint8_t column_i, uint8_t down_shift_amount) {
    bool column_copy[SCREEN_HEIGHT];
    for (size_t y = 0; y < SCREEN_HEIGHT; y++)
        column_copy[y] = screen[y * SCREEN_WIDTH + column_i];
    for (size_t y = 0; y < SCREEN_HEIGHT; y++)
        screen[y * SCREEN_WIDTH + column_i] = column_copy[(SCREEN_HEIGHT - down_shift_amount + y) % SCREEN_HEIGHT];
}

// const pointer to const file info
void both_parts(const struct file_info * const fi) {
    size_t start_of_line = 0;
    bool screen[SCREEN_NUM_PIXELS] = {};

    while (true) {
        const char* line = fi->file_bytes + start_of_line;

        // NOTE: we assume integers are always positive and never larger than a single rotation or outside the screen
        // size.
        uint8_t a;
        uint8_t b;
        size_t characters_parsed;
        if (sscanf(line, "rect %hhux%hhu %zn", &a, &b, &characters_parsed) == 2) {
            rect(screen, a, b);
        } else if (sscanf(line, "rotate row y=%hhu by %hhu %zn", &a, &b, &characters_parsed) == 2) {
            rotate_row(screen, a, b);
        } else if (sscanf(line, "rotate column x=%hhu by %hhu %zn", &a, &b, &characters_parsed) == 2) {
            rotate_column(screen, a, b);
        } else if (sscanf(line, " %*s") == EOF) {
            // this means all that was left is whitespace
            break;
        } else {
            fprintf(stderr, "ERROR: failed to match command for the line starting with: %50s\n", line);
            exit(EXIT_FAILURE);
        }

        start_of_line += characters_parsed;
    }

    printf("part 1:\n");
    size_t num_pixels_lit = 0;
    for (size_t y = 0; y < SCREEN_HEIGHT; y++)
        for (size_t x = 0; x < SCREEN_WIDTH; x++)
            if (screen[y * SCREEN_WIDTH + x])
                num_pixels_lit += 1;

    printf("num pixels lit = %zu\n", num_pixels_lit);

    printf("part 2:\n");
    // should display: EOARGPHYAO
    for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (size_t x = 0; x < SCREEN_WIDTH; x++) {
            printf("%c", screen[y * SCREEN_WIDTH + x] ? '#' : '.');
        }
        printf("\n");
    }
}

int main() {
    struct file_info fi = get_file_contents_and_size("./input_day8.txt");
    both_parts(&fi);

    free_file_info(fi);
    return EXIT_FAILURE;
}