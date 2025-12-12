#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>

#include "geblib/io.h"
#include "geblib/math.h"

unsigned int uofc(char ch) {
    return (ch - '0');
}

unsigned int largest_voltage_in_bank(const char* bank, size_t bank_size) {
    assert(bank_size >= 2);

    // An explicit assumption this function must hold. Anyone looking at the source
    // will realize it must hold when refactoring! 
#ifdef CHECK_PRECONDITIONS
    for (size_t i = 0; i < bank_size; i++)
        assert(bank[i] > '0' && bank[i] <= '9');
#endif

    unsigned int largest_first_digit = uofc(bank[bank_size - 2]);
    size_t       leftmost_index      = bank_size - 2;

    for (int32_t i = bank_size - 3; i >= 0; i--) {
        if (uofc(bank[i]) >= largest_first_digit) {
            largest_first_digit = uofc(bank[i]);
            leftmost_index = i;
        }
    }

    unsigned int largest_total = 0;
    for (size_t i = leftmost_index+1; i < bank_size; i++) {
        unsigned int tmp_total = largest_first_digit * 10 + uofc(bank[i]);
        if (tmp_total > largest_total)
            largest_total = tmp_total;
    }

    return largest_total;
}

uint64_t largest_voltage_in_bank_n_batteries(const char* bank, size_t bank_size, size_t n) {
    assert(bank_size >= 12);

#ifdef CHECK_PRECONDITIONS
    for (size_t i = 0; i < bank_size; i++)
        assert(bank[i] > '0' && bank[i] <= '9');
#endif

    uint64_t largest_bank_joltage = 0;

    size_t start = 0;
    size_t end = bank_size - (n - 1) - 1;
    for (size_t batt_i = 0; batt_i < n; batt_i++) {
        size_t largest = uofc(bank[start]);
        size_t leftmost_i = start;
        for (size_t i = start+1; i <= end; i++)
            if (uofc(bank[i]) > largest) {
                largest = uofc(bank[i]);
                leftmost_i = i;
            }

        size_t next_digit = largest * z_pow10(n - 1 - batt_i);
        largest_bank_joltage += next_digit;

        start = leftmost_i+1;
        end += 1;
    }

    return largest_bank_joltage;
}

int main() {
    struct file_info fi = get_file_contents("./input_day3.txt");

    size_t total_joltage_2batt = 0;
    size_t total_joltage_12batt = 0;
    size_t ch_i = 0;
    while (ch_i < fi.num_bytes) {
        size_t bank_size, num_chars;
        int num_captures = sscanf(fi.file_bytes + ch_i, "%*s%zn %zn", &bank_size, &num_chars);
        if (num_captures == EOF)
            break;

        total_joltage_2batt  += largest_voltage_in_bank(fi.file_bytes + ch_i, bank_size);
        total_joltage_12batt += largest_voltage_in_bank_n_batteries(fi.file_bytes + ch_i, bank_size, 12);

        ch_i += num_chars;
    }

    // < 123189
    // < 123005
    // < 103719
    // = 17405
    // oops; it pays to have an expected value at intermediate points!
    printf("(part1) total_joltage = %zu\n", total_joltage_2batt);
    // < 172986400391180
    // = 171990312704598
    // off by one error, oops!
    printf("(part2) total_joltage = %zu\n", total_joltage_12batt);
    return EXIT_SUCCESS;
}