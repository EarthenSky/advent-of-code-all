#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <iso646.h>

#include "../geblib_c/io.h"

void part1(const struct file_info *fi) {
    printf("part 1:\n");

    uint32_t num_addresses_supporting_tls = 0;
    size_t ci = 0;
    while (ci < fi->num_bytes) {
        bool has_abba_outside = false;
        bool has_abba_inside = false;
        bool in_hypernet_sequence = false;

        // we assume that each line contains only a single newline at the end
        // we also assume that hypernet sequences are never nested, and are well formed
        while (true) {
            char ch = fi->file_bytes[ci];
            if (ch == '\n' || ch == '\0') {
                // skip the newline / null termintor
                ci += 1;
                break;
            } else if (ch == '[') {
                in_hypernet_sequence = true;
                ci += 1;
            } else if (ch == ']') {
                in_hypernet_sequence = false;
                ci += 1;
            } else if (ch >= 'a' and ch <= 'z') {
                if (
                    ci < 3
                    or fi->file_bytes[ci-1] == '\n'
                    or fi->file_bytes[ci-2] == '\n'
                    or fi->file_bytes[ci-3] == '\n'
                    or fi->file_bytes[ci-1] == '['
                    or fi->file_bytes[ci-2] == '['
                    or fi->file_bytes[ci-3] == '['
                    or fi->file_bytes[ci-1] == ']'
                    or fi->file_bytes[ci-2] == ']'
                    or fi->file_bytes[ci-3] == ']'
                ) {
                    ci += 1;
                    continue;
                }
                
                bool is_abba = (
                    fi->file_bytes[ci] != fi->file_bytes[ci-1]
                    and fi->file_bytes[ci] == fi->file_bytes[ci-3]
                    and fi->file_bytes[ci-1] == fi->file_bytes[ci-2]
                );

                if (is_abba and in_hypernet_sequence) {
                    has_abba_inside = true;
                } else if (is_abba and !in_hypernet_sequence) {
                    has_abba_outside = true;
                }
                
                ci += 1;
            } else {
                fprintf(stderr, "ERROR: got unexpected char `%c` (%d)\n", ch, ch);
                exit(EXIT_FAILURE);
            }
        }

        if (has_abba_outside and !has_abba_inside)
            num_addresses_supporting_tls += 1;
    }

    printf("num addresses supporting TLS = %u\n", num_addresses_supporting_tls);
}

struct ab {
    char a;
    char b;
};

void part2(const struct file_info *fi) {
    printf("part 2:\n");

    uint32_t num_addresses_supporting_ssl = 0;
    size_t ci = 0;
    while (ci < fi->num_bytes) {
        const char* end_of_line = strstr(fi->file_bytes + ci, "\n");
        if (end_of_line == NULL)
            end_of_line = strstr(fi->file_bytes + ci, "\0");
        
        if (end_of_line == NULL) {
            fprintf(stderr, "ERROR: string malformed\n");
            exit(EXIT_FAILURE);
        }
    
        // allocate bytes equal to the size of this line, just in case
        ptrdiff_t line_size = end_of_line - (fi->file_bytes + ci);
        size_t outside_list_size = 0;
        struct ab *outside_list = malloc(line_size * sizeof *outside_list);
        size_t inside_list_size = 0;
        struct ab *inside_list = malloc(line_size * sizeof *inside_list);

        bool in_hypernet_sequence = false;

        // fill inside & outside lists
        while (true) {
            char ch = fi->file_bytes[ci];
            if (ch == '\n' || ch == '\0') {
                break;
            } else if (ch == '[') {
                in_hypernet_sequence = true;
            } else if (ch == ']') {
                in_hypernet_sequence = false;
            } else if (not (ch >= 'a' and ch <= 'z')) {
                fprintf(stderr, "ERROR: got unexpected char `%c` (%d)\n", ch, ch);
                exit(EXIT_FAILURE);
            } else if (
                ci >= 2
                and fi->file_bytes[ci-1] != '\n'
                and fi->file_bytes[ci-2] != '\n'
                and fi->file_bytes[ci-1] != '['
                and fi->file_bytes[ci-2] != '['
                and fi->file_bytes[ci-1] != ']'
                and fi->file_bytes[ci-2] != ']'
            ) {
                char a = fi->file_bytes[ci-2];
                char b = fi->file_bytes[ci-1];
                char c = fi->file_bytes[ci-0];

                bool is_aba = a != b and a == c;
                if (is_aba and in_hypernet_sequence) {
                    inside_list[inside_list_size] = (struct ab) { .a = a, .b = b };
                    inside_list_size += 1;
                } else if (is_aba and !in_hypernet_sequence) {
                    outside_list[outside_list_size] = (struct ab) { .a = a, .b = b };
                    outside_list_size += 1;
                }
            }

            ci += 1;
        }

        // skip the final newline
        ci += 1;

        // check whether there exists an ab outside where ba is inside, then add to ssl count 
        for (size_t oi = 0; oi < outside_list_size; oi++) {
            for (size_t ii = 0; ii < inside_list_size; ii++) {
                if (
                    outside_list[oi].a == inside_list[ii].b
                    and outside_list[oi].b == inside_list[ii].a
                ) {
                    num_addresses_supporting_ssl += 1;
                    goto DONE_CHECKING_SSL;
                }
            }
        }

        DONE_CHECKING_SSL:
        free(outside_list);
        free(inside_list);
    }

    printf("num addresses supporting SSL = %u\n", num_addresses_supporting_ssl);
}

int main() {
    struct file_info fi = get_file_contents("./input_day7.txt");
    part1(&fi);
    part2(&fi);
    free_file_info(fi);
    return EXIT_SUCCESS;
}