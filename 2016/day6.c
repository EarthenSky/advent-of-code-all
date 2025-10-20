#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <iso646.h>

#define NUM_LETTERS (26)

void part1(char *file_bytes, size_t num_bytes) {
    // we expect all words are the same length, every newline is a single char,
    // and that there is no final newline.
    size_t message_length;
    if (sscanf(file_bytes, "%*s%zn", &message_length) != 0) {
        fprintf(stderr, "ERROR: sscanf() failed to match string at %s:%d\n", __FILE__, __LINE__ - 1);
        exit(EXIT_FAILURE);
    }
    size_t num_lines = (num_bytes + 1) / (message_length + 1);

    char *message = malloc((message_length+1) * sizeof *message);
    message[message_length] = '\0';

    for (size_t ch_i = 0; ch_i < message_length; ch_i++) {
        uint32_t histogram[NUM_LETTERS] = { 0 };
        for (size_t line_i = 0; line_i < num_lines; line_i++) {
            char ch = file_bytes[line_i * (message_length+1) + ch_i];
            if (ch >= 'a' and ch <= 'z') {
                histogram[ch - 'a'] += 1;
            } else {
                fprintf(
                    stderr,
                    "Got unexpectected char `%c` in word at %s:%d\n",
                    ch, __FILE__, __LINE__ - 4
                );
                exit(EXIT_FAILURE);
            }
        }

        uint32_t max_value = histogram[0];
        size_t max_value_i = 0;
        for (size_t i = 1; i < NUM_LETTERS; i++) {
            if (histogram[i] > max_value) {
                max_value = histogram[i];
                max_value_i = i;
            }
        }

        message[ch_i] = 'a' + max_value_i;
    }

    printf("error corrected message = %s\n", message);
    free(message);
}

int main() {
    FILE* f = fopen("input_day6.txt", "r");

    fseek(f, 0, SEEK_END);
    size_t num_bytes = ftell(f);
    char *file_bytes = malloc((num_bytes+1) * sizeof *file_bytes);
    if (file_bytes == NULL) {
        fprintf(stderr, "ERROR: malloc() failed at %s:%d\n", __FILE__, __LINE__ - 2);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_SET);
    size_t num_bytes_read = fread(file_bytes, sizeof *file_bytes, num_bytes, f);
    if (num_bytes_read != num_bytes) {
        fprintf(stderr, "ERROR: fread() failed to read enough bytes at %s:%d\n", __FILE__, __LINE__ - 2);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    file_bytes[num_bytes] = '\0';
    
    part1(file_bytes, num_bytes);
    return EXIT_SUCCESS;
}