#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day5.exe
int main() {
    char *file_contents = get_file_contents("input_day5.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

bool is_nice(const char* string) {
    size_t num_vowels = 0;
    bool contains_duplicate_letter = false;

    size_t string_len = strlen(string);
    for (size_t i = 0; i < string_len; i++) {
        switch (string[i]) {
            case 'a':
            case 'e':
            case 'i':
            case 'o':
            case 'u':
                num_vowels += 1;
        }

        if (i > 0) {
            if (string[i] == string[i-1])
                contains_duplicate_letter = true;

            if (string[i-1] == 'a' && string[i] == 'b')
                return false;
            else if (string[i-1] == 'c' && string[i] == 'd')
                return false;
            else if (string[i-1] == 'p' && string[i] == 'q')
                return false;
            else if (string[i-1] == 'x' && string[i] == 'y')
                return false;
        } 
    }

    return num_vowels >= 3 && contains_duplicate_letter;
}

bool substring_contains_pair(char a, char b, const char* substr, size_t substr_len) {
    for (size_t i = 1; i < substr_len; i++) {
        if (substr[i-1] == a && substr[i] == b)
            return true;
    }
    return false;
}

bool is_nice_v2(const char* string) {
    bool contains_double_pair_no_overlap = false;
    bool contains_gapped_repeat = false;

    size_t string_len = strlen(string);
    for (size_t i = 0; i < string_len; i++) {
        if (i >= 2) {
            if (substring_contains_pair(string[i-1], string[i], string, i-1))
                contains_double_pair_no_overlap = true;

            if (string[i-2] == string[i])
                contains_gapped_repeat = true;
        }

        if (contains_gapped_repeat && contains_double_pair_no_overlap)
            return true;
    }

    return false;
}

// ----------------------------------------------------------------------- //

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    assert(is_nice("ugknbfddgicrmopn"));
    assert(is_nice("aaa"));
    assert(!is_nice("jchzalrnumimnmhp"));
    assert(!is_nice("haegwjzuvuyypxyu"));
    assert(is_nice("aacac"));
    assert(!is_nice("dvszwmarrgswjxmb"));

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    size_t num_nice_words = 0;
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        if (is_nice(token))
            num_nice_words += 1;
    }

    printf("num_nice_words: %llu\n", num_nice_words);
}


void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");
    
    assert(is_nice_v2("qjhvhtzxzqqjkmpb"));
    assert(is_nice_v2("xxyxx"));
    assert(!is_nice_v2("uurcxstgmygtbstg"));
    assert(!is_nice_v2("ieodomkazucvgmuy"));

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    size_t num_nice_words = 0;
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        if (is_nice_v2(token))
            num_nice_words += 1;
    }

    printf("num_nice_words: %llu\n", num_nice_words);
}