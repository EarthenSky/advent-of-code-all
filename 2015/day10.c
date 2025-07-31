#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day10.exe
int main() {
    const char *file_contents = "1321131112";
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    return 0;
}

// ----------------------------------------------------------------------- //

void look_and_say(const char *input_number, char *output_number) {
    char running_char = input_number[0];
    size_t size_of_run = 1;

    size_t output_number_i = 0;
    size_t input_number_size = strlen(input_number);
    for (size_t i = 1; i < input_number_size; i++) {
        if (input_number[i] != running_char) {
            output_number_i += sprintf(output_number+output_number_i, "%llu", size_of_run);
            output_number[output_number_i] = running_char;
            output_number_i += 1;

            running_char = input_number[i];
            size_of_run = 1;
        } else {
            size_of_run += 1;
        }
    }

    output_number_i += sprintf(output_number+output_number_i, "%llu", size_of_run);
    output_number[output_number_i] = running_char;
    output_number[output_number_i+1] = '\0';
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");
    
    const size_t MAX_NUMBER_SIZE = 1024 * 1024 * 16;

    char *running_word = malloc(MAX_NUMBER_SIZE * sizeof(char));
    char *target_word = malloc(MAX_NUMBER_SIZE * sizeof(char));
    memcpy(running_word, file_contents, file_size);
    running_word[file_size] = '\0';

    for (size_t i = 0; i < 40; i++) {
        look_and_say(running_word, target_word);
        char* tmp = running_word;
        running_word = target_word;
        target_word = tmp;

        //printf("running_word (%llu): %s\n", i, running_word);
    }

    printf("len of running_word: %llu\n", strlen(running_word));

    // printf("smallest_distance: %u\n", smallest_distance);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");
    const size_t MAX_NUMBER_SIZE = 1024 * 1024 * 16;

    char *running_word = malloc(MAX_NUMBER_SIZE * sizeof(char));
    char *target_word = malloc(MAX_NUMBER_SIZE * sizeof(char));
    memcpy(running_word, file_contents, file_size);
    running_word[file_size] = '\0';

    for (size_t i = 0; i < 50; i++) {
        // NOTE: there's a complicated but faster way to do this, but maintaining duplicate substrings, but combining
        // them with the rest of the string when their edges are the same. The larger the duplicates are, the less that
        // has to be double checked.

        // Briefly looking through a broken version of the string for 40, there hundreds of duplicates of strings as
        // large as "1123113213221121113122113121113222". And 50 for strings as large as
        // "122113111231133221121321132122311211131122211213211321222113222122211211232221121113122113121113222123211".

        look_and_say(running_word, target_word);

        char* tmp = running_word;
        running_word = target_word;
        target_word = tmp;
    }

    printf("len of running_word: %llu\n", strlen(running_word));
}