#include <stdio.h>
#include <stdlib.h> // stderr
#include <stdbool.h>
#include <stdint.h>

#include "../geblib_c/io.h"

int main() {
    struct file_info fi = get_file_contents("./input_day1.txt");
    uint32_t dial = 50;

    size_t num_zeros = 0;
    size_t num_turns_over_zero = 0;

    size_t ch_i = 0;
    while (true) {
        char ch;
        uint32_t degrees;

        size_t num_chars = 0;
        int num_results = sscanf(fi.file_bytes + ch_i, "%c%d %zn", &ch, &degrees, &num_chars);
        if (num_results == EOF) {
            break;
        } else if (num_results != 2) {
            fprintf(stderr, "ERROR: failed to read from this line");
            exit(EXIT_FAILURE);
        }

        ch_i += num_chars;

        size_t excess_degrees = degrees % 100;
        if (degrees > 100)
            num_turns_over_zero += degrees / 100;

        size_t dial_after_turn;
        if (ch == 'R') {
            dial_after_turn = (dial + excess_degrees) % 100;
            if (dial_after_turn < dial)
                num_turns_over_zero += 1;
        } else if (ch == 'L') {
            // degrees can be arbitrarily large, so we add 100 to avoid accidental overflow
            dial_after_turn = (dial + 100 - excess_degrees) % 100;
            if (dial != 0 && dial_after_turn > dial)
                num_turns_over_zero += 1;
        } else {
            fprintf(stderr, "ERROR: got invalid char");
            exit(EXIT_FAILURE);  
        }

        dial = dial_after_turn;
        if (dial == 0)
            num_zeros += 1;
    }

    printf("(part1) num_zeros = %zd\n", num_zeros);
    // < 6765
    // > 5688
    printf("(part2) num_turns_over_zero = %zd\n", num_turns_over_zero);

    return EXIT_SUCCESS;
}