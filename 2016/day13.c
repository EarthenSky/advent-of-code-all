#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../geblib_c/hashset.h"
#include "../geblib_c/io.h"
#include "../geblib_c/queue.h"

#define FAVOURITE_NUMBER 1362
#define PART1_TARGET_X 31
#define PART1_TARGET_Y 39

typedef struct vec2 {
    uint32_t x;
    uint32_t y;
} vec2_t;

typedef struct node {
    vec2_t pos;
    uint32_t steps_so_far;
} node_t;

DEFINE_HASHSET(node_t)
DEFINE_QUEUE(node_t)

bool is_wall(uint64_t x, uint64_t y) {
    // x,y is a wall iff num_ones(M) % 2 == 1
    // assume value doesn't overflow (probably...)
    uint64_t value = x*x + 3*x + 2*x*y + y + y*y + FAVOURITE_NUMBER;
    uint64_t value_before = value;

    uint8_t num_ones = 0;
    for (size_t i = 0; i < 32; i++) {
        if ((value & 0x3) == 0x3)
            num_ones += 2;
        else if ((value & 0x3) != 0x0)
            num_ones += 1;
        value = value >> 2;
    }

    return num_ones % 2 == 1;
}

size_t hash_function(const node_t* node) {
    // 12 bits for each of x and y pos
    // IMPORTANT: if steps_so_far was part of the hash, we might hash to different locations
    // and never evaluate the equality function, checking duplicate locations
    return ((size_t)node->pos.x << 12) + (size_t)node->pos.y;
}
bool equality_function(const node_t* a, const node_t* b) {
    // measures whether a position has been reached before, or whether it should be new
    return (
        a->pos.x == b->pos.x
        && a->pos.y == b->pos.y
        && a->steps_so_far >= b->steps_so_far
    );
}

void part1() {
    printf("part 1:\n");

    QUEUE_TYPE(node_t) queue = QUEUE_CREATE(node_t, 1000);
    HASHSET_TYPE(node_t) visited = HASHSET_CREATE(node_t, 1000, hash_function, equality_function);

    node_t starting_node = {
        .pos = (vec2_t){ .x = 1, .y = 1 },
        .steps_so_far = 0
    };
    QUEUE_PUSH(node_t, &queue, starting_node);

    while (true) {
        if (QUEUE_SIZE(node_t, &queue) == 0) {
            fprintf(stderr, "ERROR: could not find solution\n");
            exit(EXIT_FAILURE);
        }

        node_t current = QUEUE_POP(node_t, &queue);

        // printf("NODE %u %u %s\n", current.pos.x, current.pos.y, is_wall(current.pos.x, current.pos.y) ? "#" : ".");
        if (is_wall(current.pos.x, current.pos.y))
            continue;
        else if (HASHSET_CONTAINS(node_t, &visited, &current))
            continue;

        HASHSET_INSERT(node_t, &visited, &current);

        if (current.pos.x == PART1_TARGET_X && current.pos.y == PART1_TARGET_Y) {
            printf("min distance = %zu\n", current.steps_so_far);
            return;
        }

        node_t adjacent_nodes[4] = {
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x + 1, .y = current.pos.y },
                .steps_so_far = current.steps_so_far + 1
            },
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x, .y = current.pos.y + 1 },
                .steps_so_far = current.steps_so_far + 1
            },
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x - 1, .y = current.pos.y },
                .steps_so_far = current.steps_so_far + 1
            },
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x, .y = current.pos.y - 1 },
                .steps_so_far = current.steps_so_far + 1
            }
        };

        QUEUE_PUSH(node_t, &queue, adjacent_nodes[0]);
        QUEUE_PUSH(node_t, &queue, adjacent_nodes[1]);
        if (current.pos.x != 0)
            QUEUE_PUSH(node_t, &queue, adjacent_nodes[2]);
        if (current.pos.y != 0)
            QUEUE_PUSH(node_t, &queue, adjacent_nodes[3]);
    }
}

bool floodfill_equality_function(const node_t* a, const node_t* b) {
    // measures whether a position has been reached before, or whether it should be new
    return (
        a->pos.x == b->pos.x
        && a->pos.y == b->pos.y
    );
}

void part2() {
    printf("part 2:\n");

    QUEUE_TYPE(node_t) queue = QUEUE_CREATE(node_t, 1000);
    HASHSET_TYPE(node_t) visited = HASHSET_CREATE(node_t, 1000, hash_function, equality_function);
    HASHSET_TYPE(node_t) unique_visited = HASHSET_CREATE(node_t, 1000, hash_function, floodfill_equality_function);

    node_t starting_node = {
        .pos = (vec2_t){ .x = 1, .y = 1 },
        .steps_so_far = 0
    };
    QUEUE_PUSH(node_t, &queue, starting_node);

    while (QUEUE_SIZE(node_t, &queue) > 0) {
        node_t current = QUEUE_POP(node_t, &queue);

        if (current.steps_so_far > 50)
            continue;
        else if (is_wall(current.pos.x, current.pos.y))
            continue;
        else if (HASHSET_CONTAINS(node_t, &visited, &current))
            continue;

        HASHSET_INSERT(node_t, &visited, &current);
        HASHSET_INSERT(node_t, &unique_visited, &current);

        node_t adjacent_nodes[4] = {
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x + 1, .y = current.pos.y },
                .steps_so_far = current.steps_so_far + 1
            },
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x, .y = current.pos.y + 1 },
                .steps_so_far = current.steps_so_far + 1
            },
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x - 1, .y = current.pos.y },
                .steps_so_far = current.steps_so_far + 1
            },
            (node_t) {
                .pos = (vec2_t){ .x = current.pos.x, .y = current.pos.y - 1 },
                .steps_so_far = current.steps_so_far + 1
            }
        };

        QUEUE_PUSH(node_t, &queue, adjacent_nodes[0]);
        QUEUE_PUSH(node_t, &queue, adjacent_nodes[1]);
        if (current.pos.x != 0)
            QUEUE_PUSH(node_t, &queue, adjacent_nodes[2]);
        if (current.pos.y != 0)
            QUEUE_PUSH(node_t, &queue, adjacent_nodes[3]);
    }

    printf("(<=50 steps) visited %zu locations\n", unique_visited.size);
}

int main() {
    part1();
    part2();
    return EXIT_SUCCESS;
}