#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// #include <math.h>

#include "helpers.h"

void part1();
void part2();

int main() {
    char *file_contents = get_file_contents("input_day13.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

// This is TSP again, except we want the longest path & negative numbers exist.

// If we solve this puzzle, we also solve the corresponding max-TSP (min or max, both are hard), therefore this
// problem is also NP complete. Get proofed!

int64_t max_cost_of_round_trip(
    int64_t happiness_matrix[27][27],
    size_t first_location,
    size_t last_location,
    size_t *locations_left,
    size_t num_locations_left
) {
    if (num_locations_left == 0)
        return happiness_matrix[last_location][first_location];

    int64_t max_cost = _I64_MIN;
    for (size_t i = 0; i < num_locations_left; i++) {
        size_t *new_locations_left = malloc((num_locations_left-1) * sizeof(size_t));
        size_t new_locations_left_i = 0;
        for (size_t j = 0; j < num_locations_left; j++) {
            if (j != i) {
                new_locations_left[new_locations_left_i] = locations_left[j];
                new_locations_left_i += 1;
            }
        }

        int64_t cost = (
            happiness_matrix[last_location][locations_left[i]]
            + max_cost_of_round_trip(happiness_matrix, first_location, locations_left[i], new_locations_left, num_locations_left-1)
        );
        if (last_location == first_location) {
            printf("cost: %lld\n", cost);
            printf("cost: %lld\n", cost);
        }
        if (cost >= max_cost)
            max_cost = cost;
    }
    return max_cost;
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    // store each person by the first letter in their name
    int64_t happiness_matrix[27][27];
    memset(happiness_matrix, 0, 27 * 27 * sizeof(int64_t));
    bool name_exists[27];
    memset(name_exists, false, 27 * sizeof(bool));

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        char name_buffer_1[16+1], name_buffer_2[16+1];
        char action_buffer[4+1];
        int32_t number;
        int result = sscanf(token, "%16s would %4s %d happiness units by sitting next to %16s.", name_buffer_1, action_buffer, &number, name_buffer_2);
        if (result != 4) {
            printf("ERROR: input formatted incorrectly (only captured %d/4)\n", result);
            exit(1);
        }

        name_exists[name_buffer_1[0]-'A'] = true;

        if (strcmp(action_buffer, "lose") == 0) {
            happiness_matrix[name_buffer_1[0]-'A'][name_buffer_2[0]-'A'] -= number;
            happiness_matrix[name_buffer_2[0]-'A'][name_buffer_1[0]-'A'] -= number;
        } else {
            happiness_matrix[name_buffer_1[0]-'A'][name_buffer_2[0]-'A'] += number;
            happiness_matrix[name_buffer_2[0]-'A'][name_buffer_1[0]-'A'] += number;
        }
    }
    free(file_contents_copy);

    size_t *locations_left = malloc(27 * sizeof(size_t));
    size_t num_locations_left = 0;
    for (size_t i = 0; i < 27; i++) {
        if (name_exists[i]) {
            locations_left[num_locations_left] = i;
            num_locations_left += 1;
        }
    }

    int64_t happiness_diff = max_cost_of_round_trip(
        happiness_matrix, *locations_left, *locations_left, &(locations_left[1]), num_locations_left-1
    );
    free(locations_left);

    printf("happiness_diff: %lld\n", happiness_diff);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    // store each person by the first letter in their name
    int64_t happiness_matrix[27][27];
    memset(happiness_matrix, 0, 27 * 27 * sizeof(int64_t));
    bool name_exists[27];
    memset(name_exists, false, 27 * sizeof(bool));

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        char name_buffer_1[16+1], name_buffer_2[16+1];
        char action_buffer[4+1];
        int32_t number;
        int result = sscanf(token, "%16s would %4s %d happiness units by sitting next to %16s.", name_buffer_1, action_buffer, &number, name_buffer_2);
        if (result != 4) {
            printf("ERROR: input formatted incorrectly (only captured %d/4)\n", result);
            exit(1);
        }

        name_exists[name_buffer_1[0]-'A'] = true;

        if (strcmp(action_buffer, "lose") == 0) {
            happiness_matrix[name_buffer_1[0]-'A'][name_buffer_2[0]-'A'] -= number;
            happiness_matrix[name_buffer_2[0]-'A'][name_buffer_1[0]-'A'] -= number;
        } else {
            happiness_matrix[name_buffer_1[0]-'A'][name_buffer_2[0]-'A'] += number;
            happiness_matrix[name_buffer_2[0]-'A'][name_buffer_1[0]-'A'] += number;
        }
    }
    free(file_contents_copy);

    // HERE:
    // easy tiny fix, woo!
    name_exists["Zazzles"[0] - 'A'] = true;

    size_t *locations_left = malloc(27 * sizeof(size_t));
    size_t num_locations_left = 0;
    for (size_t i = 0; i < 27; i++) {
        if (name_exists[i]) {
            locations_left[num_locations_left] = i;
            num_locations_left += 1;
        }
    }

    int64_t happiness_diff = max_cost_of_round_trip(
        happiness_matrix, *locations_left, *locations_left, &(locations_left[1]), num_locations_left-1
    );
    free(locations_left);

    printf("happiness_diff: %lld\n", happiness_diff);
}