#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void both_parts(const char *file_contents, size_t file_size);

// Measure-Command { ./build/Debug/day16.exe }
int main() {
    char *file_contents = get_file_contents("input_day16.txt");
    size_t file_size = strlen(file_contents);
    both_parts(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

// IMPORTANT DETAILS
#define CHILDREN 3
#define CATS 7
#define SAMOYEDS 2
#define POMERANIANS 3
#define AKITAS 0
#define VIZSLAS 0
#define GOLDFISH 5
#define TREES 3
#define CARS 2
#define PERFUMES 1

bool property_matches_observations(const char *property, uint32_t value) {
    if (strcmp(property, "children") == 0) {
        return value == CHILDREN;
    } else if (strcmp(property, "cats") == 0) {
        return value == CATS;
    } else if (strcmp(property, "samoyeds") == 0) {
        return value == SAMOYEDS;
    } else if (strcmp(property, "pomeranians") == 0) {
        return value == POMERANIANS;
    } else if (strcmp(property, "akitas") == 0) {
        return value == AKITAS;
    } else if (strcmp(property, "vizslas") == 0) {
        return value == VIZSLAS;
    } else if (strcmp(property, "goldfish") == 0) {
        return value == GOLDFISH;
    } else if (strcmp(property, "trees") == 0) {
        return value == TREES;
    } else if (strcmp(property, "cars") == 0) {
        return value == CARS;
    } else if (strcmp(property, "perfumes") == 0) {
        return value == PERFUMES;
    } else {
        printf("ERROR: failed to match observation!\n");
        exit(1);
    }
}

bool property_matches_observations_v2(const char *property, uint32_t value) {
    if (strcmp(property, "children") == 0) {
        return value == CHILDREN;
    } else if (strcmp(property, "cats") == 0) {
        return value > CATS;
    } else if (strcmp(property, "samoyeds") == 0) {
        return value == SAMOYEDS;
    } else if (strcmp(property, "pomeranians") == 0) {
        return value < POMERANIANS;
    } else if (strcmp(property, "akitas") == 0) {
        return value == AKITAS;
    } else if (strcmp(property, "vizslas") == 0) {
        return value == VIZSLAS;
    } else if (strcmp(property, "goldfish") == 0) {
        return value < GOLDFISH;
    } else if (strcmp(property, "trees") == 0) {
        return value > TREES;
    } else if (strcmp(property, "cars") == 0) {
        return value == CARS;
    } else if (strcmp(property, "perfumes") == 0) {
        return value == PERFUMES;
    } else {
        printf("ERROR: failed to match observation!\n");
        exit(1);
    }
}

// kinda like a multi-dimensional knapsack problem, except that all items are the
// same size and the capacity is the product of . Not exactly sure what problem this is most similar to?
void both_parts(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        uint32_t sue_i;
        char property_0[32], property_1[32], property_2[32];
        uint32_t value_0, value_1, value_2;
        int result = sscanf(
            token, "Sue %u: %[^:]: %u, %[^:]: %u, %[^:]: %u",
            &sue_i,
            property_0, &value_0,
            property_1, &value_1,
            property_2, &value_2
        );
        if (result != 7) {
            printf("ERROR: failed to parse line (got %d)\n", result);
            exit(1);
        }

        if (
            property_matches_observations(property_0, value_0)
            && property_matches_observations(property_1, value_1)
            && property_matches_observations(property_2, value_2)
        )
            printf("(match) sue_i: %u\n", sue_i);

        if (
            property_matches_observations_v2(property_0, value_0)
            && property_matches_observations_v2(property_1, value_1)
            && property_matches_observations_v2(property_2, value_2)
        )
            printf("(match v2) sue_i: %u\n", sue_i);
    }
    free(file_contents_copy);
}
