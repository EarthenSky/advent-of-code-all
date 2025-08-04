#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void both_parts(const char *file_contents, size_t file_size);

// Measure-Command { ./build/Debug/day17.exe }
int main() {
    char *file_contents = get_file_contents("input_day17.txt");
    size_t file_size = strlen(file_contents);
    both_parts(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

// This is a divide & conquer, which takes (worst case) time T(n) = 2*T(n-1) + O(1)
// Ultimately gives us a time complexity of O(2^n) (oh heck!)
size_t number_of_combinations(size_t eggnog_left, size_t *containers_left, size_t number_of_containers) {
    if (eggnog_left == 0) {
        return 1;
    } else if (number_of_containers == 0) {
        return 0;
    }

    size_t container_capacity = containers_left[0];
    if (container_capacity > eggnog_left) {
        // unable to use this container, so we can just skip it
        return number_of_combinations(eggnog_left, containers_left+1, number_of_containers-1);
    } else {
        return (
            number_of_combinations(eggnog_left-container_capacity, containers_left+1, number_of_containers-1)
            + number_of_combinations(eggnog_left, containers_left+1, number_of_containers-1)
        );
    }
}

void number_of_combinations_of_minimum_size(
    size_t *minimum_size,
    size_t *number_of_combinations,
    size_t containers_used,
    size_t eggnog_left,
    size_t *containers_left,
    size_t number_of_containers
) {
    if (containers_used > *minimum_size) {
        return;
    } else if (eggnog_left == 0) {
        if (containers_used < *minimum_size) {
            // reset number of combinations!
            // printf("min size: %llu\n", containers_used);
            *minimum_size = containers_used;
            *number_of_combinations = 1;
        } else {
            *number_of_combinations += 1;
        }
        return;
    } else if (number_of_containers == 0) {
        return;
    }

    size_t container_capacity = containers_left[0];
    if (container_capacity <= eggnog_left) {
        // only if we can use the container!
        number_of_combinations_of_minimum_size(
            minimum_size, number_of_combinations, containers_used+1,
            eggnog_left-container_capacity, containers_left+1, number_of_containers-1
        );
    }
    number_of_combinations_of_minimum_size(
        minimum_size, number_of_combinations, containers_used,
        eggnog_left, containers_left+1, number_of_containers-1
    );
}

void both_parts(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    size_t num_containers = 0;
    size_t container_capacities[256];

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        int result = sscanf(token, "%llu", &container_capacities[num_containers]);
        assert(result == 1);
        num_containers += 1;
    }
    free(file_contents_copy);

    const size_t EGGNOG_L = 150;
    printf("number of combinations: %llu", number_of_combinations(EGGNOG_L, container_capacities, num_containers));

    printf("\npart2:\n");
    size_t minimum_size = 100000000;
    size_t number_of_combinations = 0;
    number_of_combinations_of_minimum_size(
        &minimum_size, &number_of_combinations, 0,
        EGGNOG_L, container_capacities, num_containers
    );
    // printf("minimum_size: %llu\n", minimum_size);
    printf("number of minimum combinations: %llu\n", number_of_combinations);
}
