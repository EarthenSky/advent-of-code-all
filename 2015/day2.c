#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day2.exe
int main() {
    char *file_contents = get_file_contents("input_day2.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    uint64_t total_square_feet = 0;
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        uint64_t l, w, h;
        sscanf(token, "%llux%llux%llu", &l, &w, &h);

        total_square_feet += 2 * (l*w + w*h + l*h);
        total_square_feet += min(l*w, min(w*h, l*h));
    }

    printf("total_square_feet: %llu\n", total_square_feet);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    uint64_t total_ribbon_length = 0;
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        uint64_t l, w, h;
        sscanf(token, "%llux%llux%llu", &l, &w, &h);

        total_ribbon_length += 2 * (
            l + w + h - max(l, max(w, h))
        );
        total_ribbon_length += l * w * h;
    }

    printf("total_ribbon_length: %llu\n", total_ribbon_length);
    free(file_contents_copy);
}