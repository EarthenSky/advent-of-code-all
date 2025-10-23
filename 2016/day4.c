#include <stdio.h>  // fopen(), fseek(), fread(), size_t
#include <string.h> // strcmp(), strlen(), size_t
#include <stdlib.h> // malloc(), size_t, ?
#include <stdint.h> // int32_t
#include <stdbool.h> // true, false, bool 

#include "../geblib_c/io.h"

void part1(const char *file_bytes, size_t num_bytes) {
    printf("part 1:\n");

    size_t file_offset = 0;
    uint32_t sum_of_valid_sector_ids = 0;

    while (true) {
        // name and checksum ARE null terminated
        char* name = NULL;
        unsigned int sector_id = 0;
        char* checksum = NULL;
        int num_bytes_read = 0;

        int num_matches = sscanf(
            file_bytes + file_offset,
            // Characters need to be escaped in sets, but everything except % is
            // treated as a literal outside
            // Spaces represent optional whitespace
            " %m[a-z\\-]%u[%m[a-z]]%n",
            &name, &sector_id, &checksum, &num_bytes_read
        );
        if (num_matches == EOF) {
            break;
        } else if (num_matches != 3) {
            fprintf(stderr, "ERROR: sscanf() failed to match everything. num_matches=%d\n", num_matches);
            exit(1);
        }

        file_offset += num_bytes_read;

        #define NO_ELEMENT (-1)
        int32_t histogram[26] = { 0 };
        size_t name_length = strlen(name);
        for (size_t i = 0; i < name_length; i++) {
            if (name[i] == '-') {
                continue;
            } else if (name[i] >= 'a' && name[i] <= 'z') {
                histogram[name[i] - 'a'] += 1;
            } else {
                perror("got unexpected character while filling histogram");
                exit(EXIT_FAILURE); // exit frees all our memory, so dw about it
            }
        }

        // 5 most common items from the histogram
        char top_five_letters[5+1] = { 0 };
        for (size_t i = 0; i < 5; i++) {
            int32_t max_occurences = histogram[0];
            size_t max_occurences_i = 0;
            for (size_t j = 1; j < 26; j++) {
                // strictly GT ensures the smallest letter with equal number of
                // occurences will be selected first
                if (histogram[j] > max_occurences) {
                    max_occurences = histogram[j];
                    max_occurences_i = j;
                }
            }
            top_five_letters[i] = 'a' + max_occurences_i;
            histogram[max_occurences_i] = NO_ELEMENT;
        }

        if (strcmp(top_five_letters, checksum) == 0)
            sum_of_valid_sector_ids += sector_id;

        free(name);
        free(checksum);
    }

    printf("sum_of_valid_sector_ids = %u\n", sum_of_valid_sector_ids);
}

void part2(const char *file_bytes, size_t num_bytes) {
    printf("part 2:\n");

    size_t file_offset = 0;

    while (true) {
        char* name = NULL;
        unsigned int sector_id = 0;
        char* checksum = NULL;
        int num_bytes_read = 0;

        int num_matches = sscanf(
            file_bytes + file_offset,
            // Characters need to be escaped in sets, but everything except % is
            // treated as a literal outside
            // Spaces represent optional whitespace
            " %m[a-z\\-]%u[%m[a-z]]%n",
            &name, &sector_id, &checksum, &num_bytes_read
        );
        if (num_matches == EOF) {
            break;
        } else if (num_matches != 3) {
            fprintf(stderr, "ERROR: sscanf() failed to match everything. num_matches=%d\n", num_matches);
            exit(EXIT_FAILURE);
        }

        // apply cipher
        size_t name_size = strlen(name);
        for (size_t i = 0; i < name_size; i++) {
            if (name[i] == '-') {
                name[i] = ' ';
            } else if (name[i] >= 'a' && name[i] <= 'z') {
                name[i] = 'a' + ((name[i] - 'a') + sector_id) % 26;
            } else {
                printf("ERROR: unexpected character in name\n");
                exit(1);
            }
        }

        file_offset += num_bytes_read;

        // I used Ctrl+F to find north in the list of converted sectors
        if (strncmp("north", name, 5) == 0)
            printf("%s = %d\n", name, sector_id);

        free(name);
        free(checksum);
    }
}

int main() {
    struct file_info fi = get_file_contents("./input_day4.txt");
    part1(fi.file_bytes, fi.num_bytes);
    part2(fi.file_bytes, fi.num_bytes);
    free_file_info(fi);
    return EXIT_SUCCESS;
}