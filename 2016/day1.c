#include <assert.h>
#include <stdint.h>
#include <stdio.h>  // sscanf() ?
#include <stdlib.h> // llabs()
#include <string.h>

#include "../2015/helpers.h"

enum Direction {
    NORTH = 0,
    EAST  = 1,
    SOUTH,
    WEST
};

void part1();
void part2();

int main() {
    part1();
    part2();
    return 0;
}

enum Direction rotate_left(enum Direction direction) {
    return (direction - 1) % 4;
}
enum Direction rotate_right(enum Direction direction) {
    return (direction + 1) % 4;
}

void part1() {
    printf("part 1:\n");

    const char *instructions = get_file_contents("input_day1.txt");
    size_t instructions_size = strlen(instructions);

    int64_t x = 0;
    int64_t y = 0;

    // NOTE: assume `, ` between each (direction, digit) pair. 
    enum Direction direction = NORTH;
    // ignore last command provided if it's incomplete

    size_t chi = 0;
    while ((chi+1) < instructions_size) {
        if (instructions[chi + 0] == 'L') {
            direction = rotate_left(direction);
        } else if (instructions[chi + 0] == 'R') {
            direction = rotate_right(direction);
        } else {
            // TODO: how to printf to stderr?
            printf("ERROR: got unexpected char %c that is not L or R (index %llu).\n", instructions[chi + 0], chi);
            exit(1);
        }

        // assume single digit steps
        // ASSUMPTION WAS WRONG
        int64_t num_steps = 0;
        int num_chars_matched = 0;
        sscanf(&instructions[chi + 1], "%lld%n", &num_steps, &num_chars_matched);

        if (direction == NORTH) {
            y += num_steps;
        } else if (direction == EAST) {
            x += num_steps;
        } else if (direction == SOUTH) {
            y -= num_steps;
        } else if (direction == WEST) {
            x -= num_steps;
        }

        chi += 3 + num_chars_matched;
    }

    // TODO: how to deal with the possible differnce between int64_t and long long int?
    // like what if they are a different size; what does C11 do?
    printf("shortest_distance = %lld\n", llabs(x) + llabs(y));
}

struct vec2 { int32_t x; int32_t y; };

void part2() {
    printf("part 2:\n");

    const char *instructions = get_file_contents("input_day1.txt");
    size_t instructions_size = strlen(instructions);

    int64_t x = 0;
    int64_t y = 0;

    // TODO: improve this using lines, then perhaps by some kind of hashgrid.
    const size_t MAX_NUM_SQUARES = 1024 * 1024;
    size_t position_i = 0;
    struct vec2 *position_list = malloc(MAX_NUM_SQUARES * sizeof(struct vec2));

    position_list[position_i] = (struct vec2) { .x = 0, .y = 0 };
    position_i += 1;

    // NOTE: assume `, ` between each (direction, digit) pair. 
    enum Direction direction = NORTH;
    // ignore last command provided if it's incomplete

    size_t chi = 0;
    while ((chi+1) < instructions_size) {
        if (instructions[chi + 0] == 'L') {
            direction = rotate_left(direction);
        } else if (instructions[chi + 0] == 'R') {
            direction = rotate_right(direction);
        } else {
            // TODO: how to printf to stderr?
            printf("ERROR: got unexpected char %c that is not L or R (index %llu).\n", instructions[chi + 0], chi);
            exit(1);
        }

        int32_t num_steps = 0;
        int num_chars_matched = 0;
        sscanf(&instructions[chi + 1], "%d%n", &num_steps, &num_chars_matched);

        int32_t smallest_x, largest_x;
        int32_t smallest_y, largest_y;
        if (direction == NORTH) {
            smallest_x = x;
            largest_x = x;
            smallest_y = y + 1;
            largest_y = y + num_steps;
        } else if (direction == EAST) {
            smallest_y = y;
            largest_y = y;
            smallest_x = x + 1;
            largest_x = x + num_steps;
        } else if (direction == SOUTH) {
            smallest_x = x;
            largest_x = x;
            smallest_y = y - num_steps;
            largest_y = y - 1;
        } else if (direction == WEST) {
            smallest_y = y;
            largest_y = y;
            smallest_x = x - num_steps;
            largest_x = x - 1;
        }

        /*
        // check if any x,y in range are in position list
        for (size_t i = 0; i < position_i; i++) {
            if (
                position_list[i].x <= largest_x
                && position_list[i].x >= smallest_x
            ) {
                if (
                    position_list[i].y <= largest_y
                    && position_list[i].y >= smallest_y
                ) {
                    printf("distance_to_first_duplicate = %lld\n", llabs(x) + llabs(y));
                    free(position_list);
                    return;
                }
            }
        }
        */
        
        chi += 3 + num_chars_matched;

        for (size_t i = 0; i < num_steps; i++) {
            if (direction == NORTH) {
                y += 1;
            } else if (direction == EAST) {
                x += 1;
            } else if (direction == SOUTH) {
                y -= 1;
            } else if (direction == WEST) {
                x -= 1;
            }

            // TODO: our list of positions is pretty small, but can we do this faster? (yes)
            // TODO: profile the issue first though!
            for (size_t i = 0; i < position_i; i++) {
                if (
                    position_list[i].x == x
                    && position_list[i].y == y
                ) {
                    printf("distance_to_first_duplicate = %lld\n", llabs(x) + llabs(y));
                    free(position_list);
                    return;
                }
            }

            position_list[position_i] = (struct vec2) { .x = x, .y = y };
            position_i += 1;
        }
    }

    printf("found no duplicate positions (%d total positions)\n", position_i);
    free(position_list);
}