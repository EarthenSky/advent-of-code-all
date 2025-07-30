#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day8.exe
int main() {
    char *file_contents = get_file_contents("input_day8.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}
// ----------------------------------------------------------------------- //

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    // make a copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    size_t num_string_literal_chars = 0;
    size_t in_memory_chars = 0;

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        size_t token_len = strlen(token);
        num_string_literal_chars += token_len;

        size_t unescaped_string_i = 0;
        char *unescaped_string = malloc(token_len * sizeof(char));
        memset(unescaped_string, '\0', token_len * sizeof(char));

        // assume every line has a pair of quotes we can skip
        // ALSO assume all strings are well formatted (ie. if we get \x, 2 more chars exist as well)
        size_t i = 1;
        while (i < token_len-1) {
            if (token[i] != '\\') {
                unescaped_string[unescaped_string_i] = token[i];
                unescaped_string_i += 1;
                i += 1;
            } else if (token[i+1] == '\\') {
                unescaped_string[unescaped_string_i] = '\\';
                unescaped_string_i += 1;
                i += 2;
            } else if (token[i+1] == '"') {
                unescaped_string[unescaped_string_i] = '"';
                unescaped_string_i += 1;
                i += 2;
            } else if (token[i+1] == 'x') {
                uint8_t first_char_value = (token[i+2] >= '0' && token[i+2] <= '9') ? (token[i+2] - '9') : (token[i+2] - 'a' + 10);
                uint8_t second_char_value = (token[i+3] >= '0' && token[i+3] <= '9') ? (token[i+3] - '9') : (token[i+3] - 'a' + 10);

                char char_of_hex = 16 * first_char_value + second_char_value;
                unescaped_string[unescaped_string_i] = char_of_hex;
                unescaped_string_i += 1;
                i += 4;
            }
        }

        in_memory_chars += strlen(unescaped_string);
        // printf("-> %s\n", unescaped_string);
    }
    
    printf("diff: %lld\n", (int64_t)num_string_literal_chars - (int64_t)in_memory_chars);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    // make a copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    size_t num_string_literal_chars = 0;
    size_t num_encoded_chars = 0;

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        size_t token_len = strlen(token);
        num_string_literal_chars += token_len;

        size_t newly_escaped_string_i = 1;
        char *newly_escaped_string = malloc((1 + 2 + 2 * token_len) * sizeof(char));
        memset(newly_escaped_string, '\0', (1 + 2 + 2 * token_len) * sizeof(char));
        newly_escaped_string[0] = '"';

        // assume every line has a pair of quotes we can skip
        // ALSO assume all strings are well formatted (ie. if we get \x, 2 more chars exist as well)
        size_t i = 0;
        while (i < token_len) {
            if (token[i] == '"') {
                newly_escaped_string[newly_escaped_string_i] = '\\';
                newly_escaped_string[newly_escaped_string_i+1] = '"';
                newly_escaped_string_i += 2;
                i += 1;
            } else if (token[i] == '\\') {
                newly_escaped_string[newly_escaped_string_i] = '\\';
                newly_escaped_string[newly_escaped_string_i+1] = '\\';
                newly_escaped_string_i += 2;
                i += 1;
            } else {
                newly_escaped_string[newly_escaped_string_i] = token[i];
                newly_escaped_string_i += 1;
                i += 1;
            }
        }
        newly_escaped_string[newly_escaped_string_i] = '"';

        num_encoded_chars += strlen(newly_escaped_string);
        // printf("-> %s\n", newly_escaped_string);
    }

    printf("diff: %lld\n", (int64_t)num_encoded_chars - (int64_t)num_string_literal_chars);
}