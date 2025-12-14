#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <assert.h>

#include <math.h>

#include "geblib/io.h"

// TODO: move this to a custom string library
size_t count_occurences(const char* str, size_t len, char ch) {
    size_t total = 0;
    for (size_t i = 0; i < len; i++)
        if (str[i] == ch)
            total += 1;
    return total;
}

// network groups are formed from linked lists, because it minimizes allocations
struct junction_box {
    struct junction_box* same_group_parent;
    struct junction_box* same_group_child;
    bool used;
    size_t x, y, z;
};
struct junction_box *head_of_group(struct junction_box *a) {
    struct junction_box *running = a;
    while (running->same_group_parent != NULL)
        running = running->same_group_parent;
    return running;
}
struct junction_box *tail_of_group(struct junction_box *a) {
    struct junction_box *running = a;
    while (running->same_group_child != NULL)
        running = running->same_group_child;
    return running;
}
bool in_group(const struct junction_box *head, const struct junction_box *b) {
    assert(head->same_group_parent == NULL);

    const struct junction_box *running = head;
    while (true) {
        if (
            running->x == b->x
            && running->y == b->y
            && running->z == b->z
        )
            return true;

        if (running->same_group_child == NULL)
            return false;

        running = running->same_group_child;
    }
}
size_t flood_fill(size_t start, struct junction_box *box_list) {
    size_t num_items = 0;

    struct junction_box *running = head_of_group(box_list + start);
    while (true) {
        // TODO: change `used` into `visited`
        running->used = true;
        num_items += 1;

        // TODO: assert no cycles exist at the head of this function
        if (running->same_group_child == NULL)
            return num_items;

        running = running->same_group_child;
    }
}

struct box_pair {
    size_t i, j;
    double distance;
};
double box_pair__distance(size_t i, size_t j, const struct junction_box* const box_list) {
    // compute distance on-demand
    int64_t x1 = box_list[i].x,
            y1 = box_list[i].y,
            z1 = box_list[i].z;

    int64_t x2 = box_list[j].x,
            y2 = box_list[j].y,
            z2 = box_list[j].z;

    return sqrt(
        (x1 - x2) * (x1 - x2)
        + (y1 - y2) * (y1 - y2)
        + (z1 - z2) * (z1 - z2)
    );
}

// TODO: move this to a sorting lib or something
// TODO: could I implement rust's default eq system or something using macros, or is it too hard?
int size_t_comp_asc(const void *x, const void *y) {
    size_t xs = *((const size_t*)x);
    size_t ys = *((const size_t*)y);
    if (xs > ys) return 1;
    else if (xs < ys) return -1;
    else return 0;
}
int size_t_comp_desc(const void *x, const void *y) {
    return -size_t_comp_asc(x, y);
}

int main() {
    const size_t NUM_PAIRS = 1000; 

    struct file_info fi = get_file_contents("./2025/input_day8.txt");
    // TODO: assert there is no trailing newline
    size_t num_lines = count_occurences(fi.file_bytes, fi.num_bytes, '\n') + 1;

    // TODO:
    // - use O(n^2) method to find the first n closest two non-connected boxes all at once

    size_t box_list_i = 0;
    struct junction_box* const box_list = malloc(num_lines * sizeof(*box_list));
    if (box_list == NULL) {
        exit(EXIT_FAILURE);
    }

    printf("before box_list\n");

    size_t ch_i = 0;
    while (ch_i <= fi.num_bytes) {
        size_t x,y,z;
        size_t num_chars;
        int num_captures = sscanf(fi.file_bytes + ch_i, "%zu,%zu,%zu %zn", &x, &y, &z, &num_chars);
        if (num_captures == EOF) {
            break;
        } else if (num_captures != 3) {
            exit(EXIT_FAILURE);
        }

        ch_i += num_chars;

        box_list[box_list_i] = (struct junction_box) {
            .same_group_parent = NULL,
            .same_group_child = NULL,
            .used = false,
            .x = x, .y = y, .z = z,
        };
        box_list_i += 1;
    }
    
    // add NUM_PAIRS connections
    {
        // TODO: this could be a linked list & would be slightly faster
        size_t pair_list_i = 0;
        struct box_pair* const pair_list = malloc(NUM_PAIRS * sizeof(*pair_list));
        if (pair_list == NULL) {
            exit(EXIT_FAILURE);
        }

        printf("before connections\n");

        for (size_t i = 0; i < num_lines; i++) {
            for (size_t j = 0; j < num_lines; j++) {
                if (i >= j)
                    continue;

                struct box_pair current = (struct box_pair) {
                    .i = i,
                    .j = j,
                    .distance = box_pair__distance(i, j, box_list),
                };

                size_t insert_before_loc;
                for (insert_before_loc = pair_list_i; insert_before_loc > 0; insert_before_loc--) {
                    //printf("\t\t [%zu] %f >= %f\n", insert_before_loc, current.distance, pair_list[insert_before_loc-1].distance);
                    if (current.distance >= pair_list[insert_before_loc-1].distance)
                        break;
                }
  
                if (insert_before_loc >= num_lines) {
                    continue;
                } else if (insert_before_loc == pair_list_i) {
                    pair_list[insert_before_loc] = current;
                    //printf("\tappend (%zu,%zu) at %zu - %f\n", current.i, current.j, insert_before_loc, current.distance);
                    pair_list_i += 1;
                } else {
                    size_t move_size = pair_list_i - insert_before_loc;
                    if (pair_list_i == NUM_PAIRS)
                        // if we insert into the middle of a full array, we want to overwrite the item at the end
                        move_size -= 1;

                    // O(1000) operation is slowww
                    memmove(pair_list+insert_before_loc+1, pair_list+insert_before_loc, move_size * sizeof(*pair_list));
                    //printf("\tinsert (%zu,%zu) at %zu - %f\n", current.i, current.j, insert_before_loc, current.distance);
                    pair_list[insert_before_loc] = current;

                    if (pair_list_i < NUM_PAIRS)
                        pair_list_i += 1;
                }
            }
        }

        printf("before adding connections\n");

        // add connections using our list of smallest
        assert(pair_list_i == NUM_PAIRS);
        for (size_t i = 0; i < NUM_PAIRS; i++) {
            //printf("i = %zu (%zu, %zu)\n", i, pair_list[i].i, pair_list[i].j);
            struct junction_box *i_box = box_list + pair_list[i].i;
            struct junction_box *j_box = box_list + pair_list[i].j;

            const struct junction_box *i_head = head_of_group(i_box);
            struct junction_box *j_head = head_of_group(j_box);
            // if they are in the same group, don't worry and ignore them
            if (in_group(i_head, j_box) || in_group(j_head, i_box))
                continue;

            // TODO: would a linked list class help here? Not sure if it would make code cleaner... hopefully so?
            // connect groups by connecting lists
            struct junction_box *i_tail = tail_of_group(i_box);
            i_tail->same_group_child = j_head;
            j_head->same_group_parent = i_tail;
        }

        free(pair_list);
    }

    printf("floodfill\n");

    // finally, do a flood fill search
    {
        size_t circuit_sizes_i = 0;
        size_t *circuit_sizes = malloc(num_lines * sizeof(*circuit_sizes));
        for (size_t i = 0; i < num_lines; i++) {
            if (box_list[i].used)
                continue;
            circuit_sizes[circuit_sizes_i] = flood_fill(i, box_list);
            circuit_sizes_i += 1;
        }

        assert(circuit_sizes_i >= 3);
        qsort(circuit_sizes, circuit_sizes_i, sizeof(*circuit_sizes), size_t_comp_desc);

        printf("(part1) circuit_size_product = %zu\n", circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2]);
        free(circuit_sizes);
    }
    
    free(box_list);
    free_file_info(fi);
    return EXIT_FAILURE;
}