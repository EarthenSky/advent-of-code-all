#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <iso646.h>

#include "../2015/helpers.h"

void part1(const struct file_info *fi) {
    printf("part 1:\n");

    uint32_t num_addresses_supporting_tls = 0;
    size_t ci = 0;
    while (ci < fi->num_bytes) {
        // printf("num_addresses_supporting_tls = %u (ci = %zu) (num bytes = %zu)\n", num_addresses_supporting_tls, ci, fi->num_bytes);
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
void part2() {

}

int main() {
    struct file_info fi = get_file_contents_and_size("./input_day7.txt");
    part1(&fi);
    part2(&fi);
    return EXIT_SUCCESS;
}