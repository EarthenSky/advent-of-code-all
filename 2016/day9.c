#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "../geblib_c/io.h"

void part1(const struct file_info *const fi) {
    printf("part 1:\n");
    size_t result_file_num_bytes = 0;

    size_t ch_i = 0;
    while (true) {
        if (fi->file_bytes[ch_i] == '\0')
            break;

        size_t a, b;
        size_t num_characters_parsed;
        if (sscanf(fi->file_bytes + ch_i, "(%zux%zu)%zn", &a, &b, &num_characters_parsed) == 2) {
            // We assume there will be no whitespace in either file
            result_file_num_bytes += a * b;
            ch_i += num_characters_parsed + a;
        } else {
            result_file_num_bytes += 1;
            ch_i += 1;
        }
    }

    printf("decompressed file size = %zu\n", result_file_num_bytes);
}

struct file_additions {
    size_t ch_i_added;
    size_t num_bytes_added;
};

/// @brief returns the end of the segment 
struct file_additions count_compressed_data(size_t ch_i, const char* bytes) {
    size_t a, b;
    size_t num_characters_parsed;
    if (sscanf(bytes + ch_i, "(%zux%zu)%zn", &a, &b, &num_characters_parsed) == 2) {
        // We assume there will be no whitespace anywhere
        size_t single_round_num_bytes = 0;
        size_t local_ch_i = 0;
        while (local_ch_i < a) {
            // leaf nodes will add to num_bytes, albeit quite slowly
            struct file_additions additions = count_compressed_data(ch_i + num_characters_parsed + local_ch_i, bytes);
            single_round_num_bytes += additions.num_bytes_added;
            local_ch_i += additions.ch_i_added;
        }
        return (struct file_additions) {
            .ch_i_added = num_characters_parsed + a,
            .num_bytes_added = single_round_num_bytes * b
        };
    } else {
        return (struct file_additions) { .ch_i_added = 1, .num_bytes_added = 1 };
    }
}

void part2(const struct file_info *const fi) {
    printf("part 2:\n");
    size_t result_file_num_bytes = 0;

    size_t ch_i = 0;
    while (true) {
        if (fi->file_bytes[ch_i] == '\0')
            break;

        struct file_additions additions = count_compressed_data(ch_i, fi->file_bytes);
        ch_i += additions.ch_i_added;
        result_file_num_bytes += additions.num_bytes_added;
    }

    printf("decompressed file size = %zu\n", result_file_num_bytes);
}

int main() {
    struct file_info fi = get_file_contents("./input_day9.txt");
    part1(&fi);
    part2(&fi);

    free_file_info(fi);
    return EXIT_FAILURE;
}