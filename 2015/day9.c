#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day9.exe
int main() {
    char *file_contents = get_file_contents("input_day9.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

int32_t index_in_string_list(char* string, char string_list[64][64], size_t list_size) {
    for (size_t i = 0; i < list_size; i++) {
        if (strcmp(string, string_list[i]) == 0)
            return i;
    }
    return -1;
}

uint32_t shortest_path_starting_at(
    uint32_t starting_i,
    uint32_t distance_matrix[64][64],
    uint32_t *locations_left,
    size_t num_locations_left,
    // if this is true, will find the longest path
    bool check_longest_instead
) {
    if (num_locations_left == 0) {
        return 0;
    } else {
        uint32_t running_distance;
        if (check_longest_instead)
            running_distance = 0;
        else
            running_distance = 1000 * 1000 * 1000;

        uint32_t *new_locations_left = malloc((num_locations_left - 1) * sizeof(uint32_t));
        for (size_t i = 0; i < num_locations_left; i++) {
            uint32_t next_location_i = locations_left[i];

            // -1 means we cannot take this path
            if (distance_matrix[starting_i][next_location_i] == -1)
                continue;

            // a copy of locations_left, but without item i
            uint32_t locations_i = 0;
            for (uint32_t j = 0; j < num_locations_left; j++) {
                if (j != i) {
                    new_locations_left[locations_i] = locations_left[j];
                    locations_i += 1;
                }
            }

            // recursively find distance
            if (check_longest_instead)
                running_distance = max(running_distance,
                    distance_matrix[starting_i][next_location_i]
                    + shortest_path_starting_at(next_location_i, distance_matrix, new_locations_left, num_locations_left-1, check_longest_instead)
                );
            else
                running_distance = min(running_distance,
                    distance_matrix[starting_i][next_location_i]
                    + shortest_path_starting_at(next_location_i, distance_matrix, new_locations_left, num_locations_left-1, check_longest_instead)
                );
        }
        free(new_locations_left);
        return running_distance;
    }
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    // make a copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    
    size_t num_locations = 0;
    char location_names[64][64];
    memset(location_names, 0, 64 * 64 * sizeof(char));

    uint32_t distances[64][64];
    memset(distances, -1, 64 * 64 * sizeof(uint32_t));

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        char name_a[64], name_b[64];
        memset(name_a, 0, 64 * sizeof(char));
        memset(name_b, 0, 64 * sizeof(char));
        uint32_t distance;
        sscanf(token, "%64s to %64s = %u", name_a, name_b, &distance);
        
        size_t a_i = index_in_string_list(name_a, location_names, 64);
        if (a_i == -1) {
            a_i = num_locations;
            memcpy(location_names + a_i, name_a, 64);
            num_locations += 1;
        }

        size_t b_i = index_in_string_list(name_b, location_names, 64);
        if (b_i == -1) {
            b_i = num_locations;
            memcpy(location_names + b_i, name_b, 64);
            num_locations += 1;
        }

        distances[a_i][b_i] = distance;
        distances[b_i][a_i] = distance;
    }
    free(file_contents_copy);

    uint32_t smallest_distance = 1000 * 1000 * 1000;
    for (size_t starting_i = 0; starting_i < num_locations; starting_i++) {
        // store all locations that still need to be walked to
        uint32_t locations_i = 0;
        uint32_t *locations_left = malloc((num_locations - 1) * sizeof(uint32_t));
        for (uint32_t i = 0; i < num_locations; i++) {
            if (i != starting_i) {
                locations_left[locations_i] = i;
                locations_i += 1;
            }
        }

        uint32_t shortest_path_distance = shortest_path_starting_at(starting_i, distances, locations_left, num_locations-1, false);
        free(locations_left);

        if (shortest_path_distance < smallest_distance) {
            smallest_distance = shortest_path_distance;
        }
    }
    
    printf("smallest_distance: %u\n", smallest_distance);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    
    // make a copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    
    size_t num_locations = 0;
    char location_names[64][64];
    memset(location_names, 0, 64 * 64 * sizeof(char));

    uint32_t distances[64][64];
    memset(distances, -1, 64 * 64 * sizeof(uint32_t));

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        char name_a[64], name_b[64];
        memset(name_a, 0, 64 * sizeof(char));
        memset(name_b, 0, 64 * sizeof(char));
        uint32_t distance;
        sscanf(token, "%64s to %64s = %u", name_a, name_b, &distance);
        
        size_t a_i = index_in_string_list(name_a, location_names, 64);
        if (a_i == -1) {
            a_i = num_locations;
            memcpy(location_names + a_i, name_a, 64);
            num_locations += 1;
        }

        size_t b_i = index_in_string_list(name_b, location_names, 64);
        if (b_i == -1) {
            b_i = num_locations;
            memcpy(location_names + b_i, name_b, 64);
            num_locations += 1;
        }

        distances[a_i][b_i] = distance;
        distances[b_i][a_i] = distance;
    }
    free(file_contents_copy);

    uint32_t longest_distance = 0;
    for (size_t starting_i = 0; starting_i < num_locations; starting_i++) {
        // store all locations that still need to be walked to
        uint32_t locations_i = 0;
        uint32_t *locations_left = malloc((num_locations - 1) * sizeof(uint32_t));
        for (uint32_t i = 0; i < num_locations; i++) {
            if (i != starting_i) {
                locations_left[locations_i] = i;
                locations_i += 1;
            }
        }

        uint32_t longest_path_distance = shortest_path_starting_at(starting_i, distances, locations_left, num_locations-1, true);
        free(locations_left);

        if (longest_path_distance > longest_distance) {
            longest_distance = longest_path_distance;
        }
    }
    
    printf("longest_distance: %u\n", longest_distance);
}