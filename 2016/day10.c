#include <assert.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../geblib_c/math.h"
#include "../geblib_c/io.h"

#define NO_VALUE (-1)

// TODO: move both to geblib and make it generic
struct pair__int32_t__int32_t {
    int32_t first;
    int32_t second;
};

struct binary_dag_node {
    struct binary_dag_node *left;
    struct binary_dag_node *right;
    struct pair__int32_t__int32_t value;
    bool already_gave_chips;
};
struct binary_dag_node binary_dag_node__default() {
    return (struct binary_dag_node) {
        .left = NULL,
        .right = NULL,
        .value = (struct pair__int32_t__int32_t) { .first = NO_VALUE, .second = NO_VALUE },
        .already_gave_chips = false
    };
}

#define MAX_BOT_NUMBER (210)

/// @brief propagate the values from the current bot to any children.
void give_chips_to_children(struct binary_dag_node *node, uint32_t bot_number) {
    if (node->left == NULL || node->right == NULL) {
        fprintf(stderr, "ERROR: all nodes being propagated must have left and right node pointers!\n");
        exit(EXIT_FAILURE); 
    } else if (node->value.first == NO_VALUE || node->value.second == NO_VALUE) {
        fprintf(stderr, "ERROR: trying to propagate values at bot with only 1 value\n");
        exit(EXIT_FAILURE);
    }

    int32_t min_value = min(node->value.first, node->value.second);
    int32_t max_value = max(node->value.first, node->value.second);

    if (min_value == 17 && max_value == 61)
        printf("bot_number %u compares 17 and 61\n", bot_number);

    if (node->left->value.first == NO_VALUE)
        node->left->value.first = min_value;
    else if (node->left->value.second == NO_VALUE)
        node->left->value.second = min_value;
    else {
        fprintf(stderr, "ERROR: tried to give a value (left) to a bot who already has 2 values\n");
        exit(EXIT_FAILURE);
    }

    if (node->right->value.first == NO_VALUE)
        node->right->value.first = max_value;
    else if (node->right->value.second == NO_VALUE)
        node->right->value.second = max_value;
    else {
        fprintf(stderr, "ERROR: tried to give a value (right) to a bot who already has 2 values\n");
        exit(EXIT_FAILURE);
    }
}

void construct_dag(
    const struct file_info *const fi,
    struct binary_dag_node *nodes,
    struct binary_dag_node *output_nodes
) {
    // construct DAG
    size_t ch_i = 0;
    while (ch_i < fi->num_bytes) {
        size_t characters_consumed;

        uint32_t giving_bot_number;
        char output0_kind[6+1] = {};
        uint32_t output0_number;
        char output1_kind[6+1] = {};
        uint32_t output1_number;
        if (sscanf(
            fi->file_bytes + ch_i,
            " bot %u gives low to %6[a-z] %u and high to %6[a-z] %u %zn",
            &giving_bot_number,
            output0_kind, &output0_number,
            output1_kind, &output1_number,
            &characters_consumed
        ) == 5) {
            if (strncmp("bot", output0_kind, 4) == 0) {
                nodes[giving_bot_number].left = nodes + output0_number;
            } else if (strncmp("output", output0_kind, 7) == 0) {
                nodes[giving_bot_number].left = output_nodes + output0_number;
            } else {
                fprintf(stderr, "ERROR: unknown left\n");
                exit(EXIT_FAILURE);
            }

            if (strncmp("bot", output1_kind, 4) == 0) {
                nodes[giving_bot_number].right = nodes + output1_number;
            } else if (strncmp("output", output1_kind, 7) == 0) {
                nodes[giving_bot_number].right = output_nodes + output0_number;
            } else {
                fprintf(stderr, "ERROR: unknown right\n");
                exit(EXIT_FAILURE);
            }

            ch_i += characters_consumed;
            continue;
        }

        uint32_t value;
        uint32_t receiving_bot_number;
        if (sscanf(
            fi->file_bytes + ch_i,
            " value %u goes to bot %u %zn",
            &value,
            &receiving_bot_number,
            &characters_consumed
        ) == 2) {
            if (nodes[receiving_bot_number].value.first == NO_VALUE)
                nodes[receiving_bot_number].value.first = value;
            else if (nodes[receiving_bot_number].value.second == NO_VALUE)
                nodes[receiving_bot_number].value.second = value;
            else {
                fprintf(stderr, "ERROR: passed more than 2 values to a bot\n");
                exit(EXIT_FAILURE);
            }

            ch_i += characters_consumed;
            continue;
        }

        fprintf(stderr, "ERROR: got unexpected line starting with %.50s (at %zu)\n", fi->file_bytes + ch_i, ch_i);
        exit(EXIT_FAILURE);
    }
}

void both_parts(const struct file_info *const fi) {
    printf("part 1:\n");

    struct binary_dag_node nodes[MAX_BOT_NUMBER];
    struct binary_dag_node output_nodes[MAX_BOT_NUMBER];
    for (size_t i = 0; i < MAX_BOT_NUMBER; i++) {
        nodes[i] = binary_dag_node__default();
        output_nodes[i] = binary_dag_node__default();
    }

    construct_dag(fi, nodes, output_nodes);

    while (true) {
        // 200 is small enough that this is fine
        bool all_items_processed = true;
        for (size_t i = 0; i < MAX_BOT_NUMBER; i++) {
            if (!nodes[i].already_gave_chips) {
                all_items_processed = false;
                break;
            }
        }
        if (all_items_processed)
            break;

        // NOTE: no item will ever be put in the queue more than once, so we won't need more then MAX_BOT_NUMBER elements. 
        size_t queue_start = 0;
        size_t queue_end = 0;
        struct binary_dag_node *next_bot_queue[MAX_BOT_NUMBER];

        // add external nodes to the dag
        for (size_t i = 0; i < MAX_BOT_NUMBER; i++) {
            // there may be multiple parents (a DAG!), so add them all to the queue
            if (nodes[i].already_gave_chips)
                continue;
            else if (nodes[i].value.first == NO_VALUE || nodes[i].value.second == NO_VALUE)
                continue;

            next_bot_queue[queue_end] = &nodes[i];
            queue_end += 1;
        }

        while (queue_start != queue_end) {
            struct binary_dag_node *bot = next_bot_queue[queue_start];
            queue_start += 1;

            assert((bot - nodes) >= 0);
            give_chips_to_children(bot, (uint32_t)(bot - nodes));
            bot->already_gave_chips = true;
        }
    }

    printf("part 2:\n");
    printf(
        "product of outputs 0,1,2 = %zu\n",
        output_nodes[0].value.first * output_nodes[1].value.first * output_nodes[2].value.first
    );
}

int main() {
    // ahhh, instructions are out of order
    struct file_info fi = get_file_contents("./input_day10.txt");
    both_parts(&fi);

    free_file_info(fi);
    return EXIT_FAILURE;
}