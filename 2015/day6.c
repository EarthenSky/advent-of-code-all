#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day6.exe
int main() {
    char *file_contents = get_file_contents("input_day6.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}
// ----------------------------------------------------------------------- //

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    bool *light_array = (bool *) malloc(1000 * 1000);
    memset(light_array, false, 1000 * 1000);

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        uint32_t x1,y1,x2,y2;
        if (strncmp("toggle", token, strlen("toggle")) == 0) {
            sscanf(token, "toggle %u,%u through %u,%u", &x1, &y1, &x2, &y2);
            for (size_t y = y1; y <= y2; y++) {
                for (size_t x = x1; x <= x2; x++) {
                    light_array[x + y*1000] = !light_array[x + y*1000];
                }
            }

        } else if (strncmp("turn on ", token, strlen("turn on ")) == 0) {
            sscanf(token, "turn on %u,%u through %u,%u", &x1, &y1, &x2, &y2);
            for (size_t y = y1; y <= y2; y++) {
                memset(light_array + x1 + y*1000, true, (x2 - x1) + 1);
            }

        } else if (strncmp("turn off", token, strlen("turn off")) == 0) {
            sscanf(token, "turn off %u,%u through %u,%u", &x1, &y1, &x2, &y2);
            for (size_t y = y1; y <= y2; y++) {
                memset(light_array + x1 + y*1000, false, (x2 - x1) + 1);
            }

        } else {
            printf("ERROR: unknown line: %s\n", token);
        }
    }

    size_t num_lights_on = 0;
    for (size_t i = 0; i < 1000 * 1000; i++) {
        if (light_array[i])
            num_lights_on += 1;
    } 

    printf("num_lights_on: %llu\n", num_lights_on);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    int64_t *light_array = (int64_t *) malloc(1000 * 1000 * sizeof(int64_t));
    memset(light_array, 0, 1000 * 1000 * sizeof(int64_t));

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        uint32_t x1,y1,x2,y2;
        if (strncmp("toggle", token, strlen("toggle")) == 0) {
            sscanf(token, "toggle %u,%u through %u,%u", &x1, &y1, &x2, &y2);
            for (size_t y = y1; y <= y2; y++) {
                for (size_t x = x1; x <= x2; x++) {
                    light_array[x + y*1000] += 2;
                }
            }

        } else if (strncmp("turn on ", token, strlen("turn on ")) == 0) {
            sscanf(token, "turn on %u,%u through %u,%u", &x1, &y1, &x2, &y2);
            for (size_t y = y1; y <= y2; y++) {
                for (size_t x = x1; x <= x2; x++) {
                    light_array[x + y*1000] += 1;
                }
            }

        } else if (strncmp("turn off", token, strlen("turn off")) == 0) {
            sscanf(token, "turn off %u,%u through %u,%u", &x1, &y1, &x2, &y2);
            for (size_t y = y1; y <= y2; y++) {
                for (size_t x = x1; x <= x2; x++) {
                    light_array[x + y*1000] = (light_array[x + y*1000] - 1) < 0 ? 0 : light_array[x + y*1000] - 1;
                }
            }

        } else {
            printf("ERROR: unknown line: %s\n", token);
        }
    }

    size_t total_brightness = 0;
    for (size_t i = 0; i < 1000 * 1000; i++) {
        total_brightness += light_array[i];
    } 

    printf("total_brightness: %llu\n", total_brightness);
}