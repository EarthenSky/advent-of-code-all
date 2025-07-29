#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// #include <math.h>

#include "helpers.h"

void part1();
void part2();

// cmake --build build; ./build/Debug/day3.exe
int main() {
    char *file_contents = get_file_contents("input_day3.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //
// baby hashmap impl

struct HashTableEntry {
    bool is_set;
    int32_t x;
    int32_t y;
    uint32_t count;
};

size_t hash_of_xy(int32_t x, int32_t y) {
    return (size_t) (x * 3731) ^ (size_t) (y * 103101);
}

// return the first x,y match or unset item
int32_t get_hash_table_index_of(
    const struct HashTableEntry *hash_table,
    const uint32_t hash_table_size,
    int32_t x, int32_t y
) {
    size_t original_index = hash_of_xy(x, y) % hash_table_size;
    size_t index = original_index;
    struct HashTableEntry entry = hash_table[index];
    if (!entry.is_set) {
        return index;
    } else {
        while (!(entry.x == x && entry.y == y)) {
            index = (index + 1) % hash_table_size;
            entry = hash_table[index];
            if (!entry.is_set) {
                return index;
            } else if (index == original_index) {
                printf("ERROR: hashmap is full\n");
                return -1;
            }
        }
        return index;
    }
}

// ----------------------------------------------------------------------- //

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");
    // the file has ~10k chars, so we add extra for safety
    const uint32_t HASH_TABLE_SIZE = 50000;
    struct HashTableEntry *hash_table = (struct HashTableEntry *) malloc(HASH_TABLE_SIZE * sizeof(struct HashTableEntry));
    memset((void *) hash_table, 0, HASH_TABLE_SIZE * sizeof(struct HashTableEntry));

    int32_t x = 0;
    int32_t y = 0;

    uint32_t index = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, x, y);
    hash_table[index] = (struct HashTableEntry){ true, 0, 0, 1 };

    for (size_t i = 0; i < file_size; i++) {
        switch (file_contents[i]) {
            case '^':
                y += 1;
                break;
            case 'v':
                y -= 1;
                break;
            case '>':
                x += 1;
                break;
            case '<':
                x -= 1;
                break;
            default:
                printf("ERROR: unexpected character %c\n", file_contents[i]);
                exit(2);
        }

        uint32_t index = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, x, y);
        struct HashTableEntry entry = hash_table[index];
        if (entry.is_set) {
            hash_table[index] = (struct HashTableEntry) {
                true, x, y, entry.count + 1
            };
        } else {
            hash_table[index] = (struct HashTableEntry) {
                true, x, y, 1
            };
        }
    }

    size_t num_houses_with_presents = 0;
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        struct HashTableEntry entry = hash_table[i];
        if (entry.is_set && entry.count > 0) {
            num_houses_with_presents += 1;
        }
    }

    printf("num_houses_with_presents: %llu\n", num_houses_with_presents);
}

void deliver_half_presents(
    const char *file_contents,
    const size_t file_size,
    const bool offset,
    struct HashTableEntry *hash_table,
    const uint32_t hash_table_size
) {
    int32_t x = 0;
    int32_t y = 0;

    uint32_t index = get_hash_table_index_of(hash_table, hash_table_size, x, y);
    struct HashTableEntry entry = hash_table[index];
    if (entry.is_set) {
        hash_table[index] = (struct HashTableEntry) {
            true, x, y, entry.count + 1
        };
    } else {
        hash_table[index] = (struct HashTableEntry) {
            true, x, y, 1
        };
    }

    for (size_t i = offset; i < file_size; i += 2) {
        if (i >= file_size)
            break;

        // printf("%c\t", file_contents[i]);
        switch (file_contents[i]) {
            case '^':
                y += 1;
                break;
            case 'v':
                y -= 1;
                break;
            case '>':
                x += 1;
                break;
            case '<':
                x -= 1;
                break;
            default:
                printf("ERROR: unexpected character %c\n", file_contents[i]);
                exit(2);
        }

        uint32_t index = get_hash_table_index_of(hash_table, hash_table_size, x, y);
        struct HashTableEntry entry = hash_table[index];
        if (entry.is_set) {
            hash_table[index] = (struct HashTableEntry) {
                true, x, y, entry.count + 1
            };
        } else {
            hash_table[index] = (struct HashTableEntry) {
                true, x, y, 1
            };
        }
    }
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    // the file has ~10k chars, so we add extra for safety
    const uint32_t HASH_TABLE_SIZE = 50000;
    struct HashTableEntry *hash_table = (struct HashTableEntry *) malloc(HASH_TABLE_SIZE * sizeof(struct HashTableEntry));
    memset((void *) hash_table, 0, HASH_TABLE_SIZE * sizeof(struct HashTableEntry));

    deliver_half_presents(file_contents, file_size, 0, hash_table, HASH_TABLE_SIZE);
    deliver_half_presents(file_contents, file_size, 1, hash_table, HASH_TABLE_SIZE);

    size_t num_houses_with_presents = 0;
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        struct HashTableEntry entry = hash_table[i];
        if (entry.is_set && entry.count > 0) {
            num_houses_with_presents += 1;
        }
    }

    printf("num_houses_with_presents: %llu\n", num_houses_with_presents);
}