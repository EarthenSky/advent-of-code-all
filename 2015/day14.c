#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void part1();
void part2();

int main() {
    char *file_contents = get_file_contents("input_day14.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    uint64_t max_distance = 0;
    const uint64_t TIME_LIMIT = 2503;

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        uint64_t fly_speed;
        uint64_t fly_s;
        uint64_t rest_s;
        sscanf(token, "%*s can fly %llu km/s for %llu seconds, but then must rest for %llu seconds.", &fly_speed, &fly_s, &rest_s);
        
        uint64_t num_repetitions = TIME_LIMIT / (fly_s + rest_s);
        uint64_t remainder = TIME_LIMIT % (fly_s + rest_s);

        uint64_t distance_travelled = (num_repetitions * fly_speed * fly_s) + min(remainder, fly_s) * fly_speed;
        max_distance = max(max_distance, distance_travelled);
    }
    free(file_contents_copy);

    printf("max_distance: %lld\n", max_distance);
}

struct ReindeerSpecs {
    uint64_t fly_speed;
    uint64_t fly_s;
    uint64_t rest_s;
};

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    uint64_t max_distance = 0;
    const uint64_t TIME_LIMIT = 2503;

    size_t num_reindeer = 0;
    struct ReindeerSpecs reindeer_specs[27];
    size_t reindeer_distance[27];
    size_t reindeer_points[27];

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        int result = sscanf(
            token,
            "%*s can fly %llu km/s for %llu seconds, but then must rest for %llu seconds.",
            &reindeer_specs[num_reindeer].fly_speed,
            &reindeer_specs[num_reindeer].fly_s,
            &reindeer_specs[num_reindeer].rest_s
        );
        if (result != 3) {
            printf("ERROR: parsing failed!");
            exit(2);
        }

        reindeer_distance[num_reindeer] = 0;
        reindeer_points[num_reindeer] = 0;
        num_reindeer += 1;
    }
    free(file_contents_copy);

    for (size_t second_i = 0; second_i < TIME_LIMIT; second_i++) {
        for (size_t ri = 0; ri < num_reindeer; ri++) {
            uint64_t remainder = second_i % (reindeer_specs[ri].fly_s + reindeer_specs[ri].rest_s);
            if (remainder < reindeer_specs[ri].fly_s)
                reindeer_distance[ri] += reindeer_specs[ri].fly_speed;
        }

        size_t furthest_reindeer_distance = 0;
        for (size_t ri = 0; ri < num_reindeer; ri++)
            furthest_reindeer_distance = max(furthest_reindeer_distance, reindeer_distance[ri]);

        // allow both reindeer in a tie to get points
        for (size_t ri = 0; ri < num_reindeer; ri++)
            if (reindeer_distance[ri] == furthest_reindeer_distance)
                reindeer_points[ri] += 1;
    }

    size_t most_points = 0;
    for (size_t ri = 0; ri < num_reindeer; ri++)
        most_points = max(most_points, reindeer_points[ri]);

    printf("most_points: %lld\n", most_points);
}