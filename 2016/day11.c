#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../geblib_c/vector.h"

#define NUM_ITEMS (5)
#define NO_ELEMENT (-1)

typedef struct item {
    const char *name;
    int generator_layer;
    int chip_layer;
} item_t;

typedef struct search_node {
    item_t state[NUM_ITEMS];
    size_t elevator;
    size_t steps;
} search_node_t;

DEFINE_VECTOR(search_node_t)

bool is_fried(const item_t *const items) {
    for (size_t i = 0; i < NUM_ITEMS; i++) {
        if (items[i].chip_layer == items[i].generator_layer)
            continue;

        for (size_t j = 0; j < NUM_ITEMS; j++) {
            if (i == j)
                continue;

            if (items[i].chip_layer == items[j].generator_layer)
                return true;
        }
    }

    return false;
}

bool is_solution(const item_t *const items) {
    for (size_t i = 0; i < NUM_ITEMS; i++)
        if (items[i].generator_layer != 3 || items[i].chip_layer != 3)
            return false;
    return true;
}

void add_frontier_node(
    const search_node_t *parent_node,
    VECTOR_TYPE(search_node_t) *frontier,
    int generator_i,
    int generator_j,
    int chip_i,
    int chip_j,
    int elevator_diff
) {
    search_node_t new_node;
    memcpy(new_node.state, parent_node->state, sizeof new_node.state);
    if (generator_i >= 0)
        new_node.state[generator_i].generator_layer += elevator_diff;
    if (generator_j >= 0)
        new_node.state[generator_j].generator_layer += elevator_diff;
    if (chip_i >= 0)
        new_node.state[chip_i].chip_layer += elevator_diff;
    if (chip_j >= 0)
        new_node.state[chip_j].chip_layer += elevator_diff;

    new_node.elevator = parent_node->elevator + elevator_diff;
    new_node.steps = parent_node->steps + 1;
    // BFS !
    VECTOR_INSERT(search_node_t, frontier, 0, new_node);
}

void add_frontier_nodes_in_direction(
    const search_node_t *parent_node,
    VECTOR_TYPE(search_node_t) *frontier,
    int elevator_diff
) {
    const item_t *items = parent_node->state;

    // single object case
    for (size_t i = 0; i < NUM_ITEMS; i++) {
        // if an item is symmetrical, only pick one of the choices.
        bool found_symmetry_i = false;
        for (size_t k = 0; k < i; k++) {
            if (
                items[i].chip_layer == items[k].chip_layer
                && items[i].generator_layer == items[k].generator_layer
            ) {
                found_symmetry_i = true;
                break;
            }
        }
        if (found_symmetry_i)
            continue;

        if (items[i].generator_layer == parent_node->elevator)
            add_frontier_node(parent_node, frontier, i, NO_ELEMENT, NO_ELEMENT, NO_ELEMENT, elevator_diff);
        if (items[i].chip_layer == parent_node->elevator)
            add_frontier_node(parent_node, frontier, NO_ELEMENT, NO_ELEMENT, i, NO_ELEMENT, elevator_diff);
    }

    // double object case
    for (size_t i = 0; i < NUM_ITEMS; i++) {
        // if an item is symmetrical, only pick one of the choices.
        bool found_symmetry_i = false;
        for (size_t k = 0; k < i; k++) {
            if (
                items[i].chip_layer == items[k].chip_layer
                && items[i].generator_layer == items[k].generator_layer
            ) {
                found_symmetry_i = true;
                break;
            }
        }
        if (found_symmetry_i)
            continue;

        for (size_t j = i+1; j < NUM_ITEMS; j++) {
            // if an item is symmetrical, only pick one of the choices.
            bool found_symmetry_j = false;
            for (size_t k = i+1; k < j; k++) {
                if (
                    items[j].chip_layer == items[k].chip_layer
                    && items[j].generator_layer == items[k].generator_layer
                ) {
                    found_symmetry_j = true;
                    break;
                }
            }
            if (found_symmetry_j)
                continue;

            if (
                items[i].generator_layer == parent_node->elevator
                && items[j].generator_layer == parent_node->elevator
            )
                add_frontier_node(parent_node, frontier, i, j, NO_ELEMENT, NO_ELEMENT, elevator_diff);

            if (
                items[i].generator_layer == parent_node->elevator
                && items[j].chip_layer == parent_node->elevator
            )
                add_frontier_node(parent_node, frontier, i, NO_ELEMENT, j, NO_ELEMENT, elevator_diff);

            if (
                items[i].chip_layer == parent_node->elevator
                && items[j].generator_layer == parent_node->elevator
            )
                add_frontier_node(parent_node, frontier, j, NO_ELEMENT, i, NO_ELEMENT, elevator_diff);

            if (
                items[i].chip_layer == parent_node->elevator
                && items[j].chip_layer == parent_node->elevator
            )
                add_frontier_node(parent_node, frontier, NO_ELEMENT, NO_ELEMENT, i, j, elevator_diff);
        }

        if (
            items[i].chip_layer == parent_node->elevator
            && items[i].generator_layer == parent_node->elevator
        )
            add_frontier_node(parent_node, frontier, i, NO_ELEMENT, NO_ELEMENT, i, elevator_diff);
    }
}

bool mostly_equal(const search_node_t *a, const search_node_t *b) {
    if (a->elevator != b->elevator)
        return false;

    for (size_t i = 0; i < NUM_ITEMS; i++) {
        if (a->state[i].chip_layer != b->state[i].chip_layer)
            return false;
        else if (a->state[i].generator_layer != b->state[i].generator_layer)
            return false;
    }

    return true;
}

bool already_visited(const VECTOR_TYPE(search_node_t) *vec, const search_node_t *node) {
    for (size_t i = 0; i < vec->size; i++) {
        // if there's a node we've already visited, but it's smaller then take it!
        if (mostly_equal(&vec->data[i], node) && node->steps >= vec->data[i].steps)
            return true;
    }

    return false;
}

void part1() {
    printf("part 1:\n");

    /*
    item_t items[NUM_ITEMS] = {
        (item_t) { .name = "hydrogen", .generator_layer = 1, .chip_layer = 0 },
        (item_t) { .name = "lithium", .generator_layer = 2, .chip_layer = 0 },
    };
    */

    item_t items[NUM_ITEMS] = {
        (item_t) { .name = "promethium", .generator_layer = 0, .chip_layer = 0 },
        (item_t) { .name = "cobalt", .generator_layer = 1, .chip_layer = 2 },
        (item_t) { .name = "curium", .generator_layer = 1, .chip_layer = 2 },
        (item_t) { .name = "ruthenium", .generator_layer = 1, .chip_layer = 2 },
        (item_t) { .name = "plutonium", .generator_layer = 1, .chip_layer = 2 },
    };

    // TODO: make visited into a hashmap
    VECTOR_TYPE(search_node_t) visited = VECTOR_CREATE(search_node_t, 1000);
    VECTOR_TYPE(search_node_t) frontier = VECTOR_CREATE(search_node_t, 1000);
    search_node_t initial_node;
    memcpy(initial_node.state, items, sizeof items);
    initial_node.elevator = 0;
    initial_node.steps = 0;
    VECTOR_ADD(search_node_t, &frontier, initial_node);

    // TODO: for BFS, use a queue, not a vector
    size_t smallest_solution = 1024 * 1024;
    size_t num_processed = 0;
    while (frontier.size != 0) {
        search_node_t node = VECTOR_POP(search_node_t, &frontier);
        if (is_fried(node.state)) {
            continue;
        } else if (already_visited(&visited, &node)) {
            continue;
        } 
        
        VECTOR_ADD(search_node_t, &visited, node);
        if (is_solution(node.state)) {
            if (node.steps < smallest_solution)
                smallest_solution = node.steps;
            printf("found = %zu\n", node.steps);
            continue;
        }

        // down case
        if (node.elevator != 0)
            add_frontier_nodes_in_direction(&node, &frontier, -1);

        // up case
        if (node.elevator != 3)
            add_frontier_nodes_in_direction(&node, &frontier, 1);

        if (num_processed % 1000 == 0)
            printf("frontier size = %zu (steps = %zu) (num_processed = %zu)\n", frontier.size, node.steps, num_processed);

        num_processed += 1;
    }

    // 53 is wrong...
    // 28 is wrong...
    // 33 is correct -> it took 7 minutes but I am tired so I don't care
    printf("smallest_solution = %zu\n", smallest_solution);
}

int main() {
    part1();
    return EXIT_SUCCESS;
}