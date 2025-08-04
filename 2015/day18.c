#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// #include <math.h>

#include "helpers.h"

void both_parts(const char *file_contents, size_t file_size);

// Measure-Command { ./build/Debug/day18.exe }
int main() {
    char *file_contents = get_file_contents("input_day18.txt");
    size_t file_size = strlen(file_contents);
    both_parts(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

size_t num_adjacent_active_lights(bool *led_matrix, size_t x, size_t y) {
    size_t number = 0;
    if (x != 0) {
        number += led_matrix[100 * y + (x-1)];
    }
    if (x != 99) {
        number += led_matrix[100 * y + (x+1)];
    }
    if (y != 0) {
        number += led_matrix[100 * (y-1) + x];
    }
    if (y != 99) {
        number += led_matrix[100 * (y+1) + x];
    }
    if (x != 0 && y != 0) {
        number += led_matrix[100 * (y-1) + (x-1)];
    }
    if (x != 0 && y != 99) {
        number += led_matrix[100 * (y+1) + (x-1)];
    }
    if (x != 99 && y != 0) {
        number += led_matrix[100 * (y-1) + (x+1)];
    }
    if (x != 99 && y != 99) {
        number += led_matrix[100 * (y+1) + (x+1)];
    }
    return number;
}

// this is conway's game of life, but modified a lil (cellular automata!)
void both_parts(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    bool led_matrix[100*100];
    bool buffer[100*100];

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    size_t y = 0;
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        size_t token_len = strlen(token);
        for (size_t x = 0; x < token_len; x++) {
            led_matrix[y * 100 + x] = (token[x] == '#');
        }
        y += 1;
    }
    free(file_contents_copy);

    // perform animation
    bool *next_led_matrix = buffer;
    bool *last_led_matrix = led_matrix;
    for (size_t step_i = 0; step_i < 100; step_i++) {
        for (size_t led_y = 0; led_y < 100; led_y++) {
            for (size_t led_x = 0; led_x < 100; led_x++) {
                size_t num_lights = num_adjacent_active_lights(last_led_matrix, led_x, led_y);
                if (last_led_matrix[led_y * 100 + led_x]) {
                    next_led_matrix[led_y * 100 + led_x] = (num_lights == 2 || num_lights == 3);
                } else {
                    next_led_matrix[led_y * 100 + led_x] = (num_lights == 3);
                }
            }
        }

        // swap buffers
        bool *tmp = next_led_matrix;
        next_led_matrix = last_led_matrix;
        last_led_matrix = tmp;
    }

    size_t num_lights_on = 0;
    for (size_t yi = 0; yi < 100; yi++) {
        for (size_t xi = 0; xi < 100; xi++) {
            num_lights_on += (size_t) last_led_matrix[yi * 100 + xi];
        }
    }
    printf("num_lights_on: %llu\n", num_lights_on);

    printf("\npart2:\n");
}
