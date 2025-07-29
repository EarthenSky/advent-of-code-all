#include <stdio.h>

#include "helpers.h"

void part1();
void part2();

int main() {
    part1();
    part2();
    return 0;
}

void part1() {
    char* file_contents = get_file_contents("input_day1.txt");
    size_t file_size = strlen(file_contents);
    
    long long floor_number = 0;
    for (size_t i = 0; i < file_size; i++) {
        floor_number += file_contents[i] == '(' ? 1 : -1;
    }

    printf("floor_number: %lld\n", floor_number);
}

void part2() {
    char* file_contents = get_file_contents("input_day1.txt");
    size_t file_size = strlen(file_contents);
    
    long long floor_number = 0;
    size_t first_index_of_basement; // 1-indexed

    for (size_t i = 0; i < file_size; i++) {
        floor_number += file_contents[i] == '(' ? 1 : -1;

        if (floor_number == -1) {
            first_index_of_basement = i+1;
            break;
        }
    }

    printf("first_index_of_basement: %llu\n", first_index_of_basement);
}