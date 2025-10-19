#include <stdio.h>  // fopen(), size_t
#include <string.h> // strcmp(), strlen(), size_t
#include <stdlib.h> // malloc(), size_t, ?
#include <stdint.h> // int32_t
#include <stdbool.h> // true, false, bool 

#include <errno.h> // errno 

void part1(const char *file_bytes, size_t num_bytes) {
    printf("part1:\n");

    size_t file_offset = 0;
    uint32_t sum_of_valid_sector_ids = 0;

    while (true) {
        char* name = NULL;
        unsigned int sector_id = 0;
        char* checksum = NULL;
        int num_bytes_read = 0;

        errno = 0;
        int result = sscanf(
            file_bytes + file_offset,
            "%m[a-z\\-]+%u\\[%m[a-z]+\\]%n",
            &name, &sector_id, &checksum, &num_bytes_read
        );
        if (result == EOF) {
            break;
        } else if (result != 4) {
            printf("ERROR: sscanf failed to match something. result = %d\n", result);
            exit(1);
        } else if (errno != 0) {
            perror("sscanf");
        }

        if (name == NULL) {
            perror("IDK why this is happening");
            exit(1);
        } else if (checksum == NULL) {
            perror("WHYYYYYYYYYY");
            exit(1);
        }
            
        file_offset += num_bytes_read;

        #define NO_ELEMENT (-1)
        // TODO: is this zero init?
        // TODO: what header does int32_t come from?
        int32_t histogram[27] = {0};
        // TODO: is name null terminated?
        printf("before strlen\n");
        size_t name_length = strlen(name);
        printf("after strlen\n");
        for (size_t i = 0; i < name_length; i++) {
            if (name[i] == '-') {
                continue;
            } else if (name[i] >= 'a' && name[i] <= 'z') {
                histogram[name[i] - 'a'] += 1;
            } else {
                perror("got unexpected character while filling histogram");
                exit(1); // exit frees all our memory, so dw about it
            }
        }
        printf("after loop\n");

        // 5 most common items from the histogram
        char top_five_letters[6] = { '\0' };
        for (size_t i = 0; i < 5; i++) {
            uint32_t max_occurences = histogram[0];
            size_t max_occurences_i = 0;
            for (size_t j = 1; j < 27; j++) {
                // strictly GT ensures the smallest letter with equal number of
                // occurences will be selected first
                if (histogram[j] > max_occurences) {
                    max_occurences = histogram[j];
                    max_occurences_i = j;
                }
            }
            top_five_letters[i] = max_occurences_i;
        }
        printf("after loop2\n");

        printf("len %llu\n", strlen(top_five_letters));
        printf("len %llu, len2 %llu\n", strlen(top_five_letters), strlen(checksum));

        if (strcmp(top_five_letters, checksum) == 0)
            sum_of_valid_sector_ids += sector_id;

        printf("after cmp\n");

        free(name);
        free(checksum);

        printf("after free\n");
    }

    printf("sum_of_valid_sector_ids = %u\n", sum_of_valid_sector_ids);
}

int main() {
    FILE* f = fopen("./input_day4.txt", "r");
    if (f == NULL) {
        // TODO: can I get more info about the failure reason?
        perror("failed to open");
        return 1;
    }

    // TODO: can these function fail too?
    fseek(f, 0, SEEK_END);
    size_t num_bytes = ftell(f);
    char *file_bytes = malloc((num_bytes + 1) * sizeof(char));
    if (file_bytes == NULL) {
        perror("failed to malloc");
        return 1;
    }

    fseek(f, 0, SEEK_SET);
    // TODO: what does fread do?
    size_t num_bytes_read = fread(file_bytes, 1, num_bytes, f);
    if (num_bytes_read != num_bytes) {
        perror("fread() didn't read the correct number of bytes");
        fclose(f);
        free(file_bytes);
        return 1;
    } else {
        fclose(f);
        file_bytes[num_bytes] = '\0';
    }

    part1(file_bytes, num_bytes);

    free(file_bytes);
    return 0;
}