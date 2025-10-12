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

void part2() {
    printf("part 2:\n");

}