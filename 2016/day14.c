#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../geblib_c/queue.h"
#include "../geblib_c/md5.h"

#define SALT "jlmsuwbz"
// #define SALT "abc"

#define MAX_U64_SIZE 32

DEFINE_QUEUE(uint16_t)

/// returns a u4
uint8_t hex_digit_i(struct bits_128 hash, size_t i) {
    if (i % 2 == 0)
        return hash.bytes[i / 2] >> 4; 
    else
        return hash.bytes[i / 2] & 0x0f;
}

int8_t first_triplet(struct bits_128 hash) {
    for (size_t i = 0; i < 32 - 2; i++)
        if (
            hex_digit_i(hash, i+0) == hex_digit_i(hash, i+1)
            && hex_digit_i(hash, i+1) == hex_digit_i(hash, i+2)
        )
            return hex_digit_i(hash, i+0);

    return -1;
}

/// @return each bit represents whether a 5 of a kind of that style exists
uint16_t count_5kind_hashes(struct bits_128 hash) {
    uint16_t result = 0x0000;
    for (size_t i = 0; i < 32 - 4; i++) {
        if (
            hex_digit_i(hash, i+0) == hex_digit_i(hash, i+1)
            && hex_digit_i(hash, i+1) == hex_digit_i(hash, i+2)
            && hex_digit_i(hash, i+2) == hex_digit_i(hash, i+3)
            && hex_digit_i(hash, i+3) == hex_digit_i(hash, i+4)
        )
            result |= (0x1 << hex_digit_i(hash, i+0));
    }

    /*
    printf("got 5kinds %hx for\n", result);
    printf(
        "%hhx%hhx%hhx%hhx %hhx%hhx%hhx%hhx %hhx%hhx%hhx%hhx %hhx%hhx%hhx%hhx\n",
        hash.bytes[0], hash.bytes[1], hash.bytes[2], hash.bytes[3],
        hash.bytes[4], hash.bytes[5], hash.bytes[6], hash.bytes[7],
        hash.bytes[8], hash.bytes[9], hash.bytes[10], hash.bytes[11],
        hash.bytes[12], hash.bytes[13], hash.bytes[14], hash.bytes[15]
    );
    */

    return result;
}

void part1() {
    printf("part 1:\n");

    char *to_hash_str = malloc(strlen(SALT) * sizeof(char) + MAX_U64_SIZE + MAX_MD5_EXTRA_BYTES_NEEDED);
    if (to_hash_str == NULL) exit(EXIT_FAILURE);

    uint16_t lookahead_counts[16] = {};
    for (size_t i = 1; i < 1000+1; i++) {
        memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
        sprintf(to_hash_str + strlen(SALT), "%zu", i);
    
        struct bits_128 hash = md5_inplace(to_hash_str, strlen(to_hash_str));
        uint16_t bits = count_5kind_hashes(hash);
        for (size_t i = 0; i < 16; i++)
            if ((bits & (0x1 << i)) != 0)
                lookahead_counts[i] += 1;
    }

    /*
    printf("\n");
    for (size_t i = 0; i < 16; i++)
        printf("lookahead_counts[%zu] = %hu\n", i, lookahead_counts[i]);
    printf("======\n");
    */

    size_t hash_i = 0;
    size_t num_keys_found = 0;

    while (true) {
        memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
        sprintf(to_hash_str + strlen(SALT), "%zu", hash_i); // TODO: CAN THIS FAIL?

        {
            struct bits_128 hash = md5_inplace(to_hash_str, strlen(to_hash_str));
            int8_t triplet = first_triplet(hash);
            if (triplet != -1 && lookahead_counts[triplet] != 0)
                num_keys_found += 1;

            if (num_keys_found == 64)
                break;

            hash_i += 1;
        }

        // remove last oldest item from the lookahead
        {
            memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
            sprintf(to_hash_str + strlen(SALT), "%zu", hash_i);
            struct bits_128 hash = md5_inplace(to_hash_str, strlen(to_hash_str));
            uint16_t bits = count_5kind_hashes(hash);
            for (size_t i = 0; i < 16; i++)
                if ((bits & (0x1 << i)) != 0)
                    lookahead_counts[i] -= 1;
        }

        {
            // add next item to the lookahead
            memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
            sprintf(to_hash_str + strlen(SALT), "%zu", hash_i + 1000);
            struct bits_128 hash = md5_inplace(to_hash_str, strlen(to_hash_str));
            uint16_t bits = count_5kind_hashes(hash);
            for (size_t i = 0; i < 16; i++)
                if ((bits & (0x1 << i)) != 0)
                    lookahead_counts[i] += 1;
        }
    }

    free(to_hash_str);
    printf("index of 64th key %zu\n", hash_i);
}

struct bits_128 stretched_hash(char *message) {
    const int NUM_TIMES = 2016;
    char* input = message;
    struct bits_128 hash = md5_inplace(message, strlen(message));
    for (size_t i = 0; i < NUM_TIMES; i++) {
        char last_hash_as_string[32 + MAX_MD5_EXTRA_BYTES_NEEDED];
        sprintf(
            last_hash_as_string,
            "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
            hash.bytes[0], hash.bytes[1], hash.bytes[2], hash.bytes[3],
            hash.bytes[4], hash.bytes[5], hash.bytes[6], hash.bytes[7],
            hash.bytes[8], hash.bytes[9], hash.bytes[10], hash.bytes[11],
            hash.bytes[12], hash.bytes[13], hash.bytes[14], hash.bytes[15]
        );
        hash = md5_inplace(last_hash_as_string, strlen(last_hash_as_string));
    }

    return hash;
}

void part2() {
    printf("part 2:\n");

    char *to_hash_str = malloc(strlen(SALT) * sizeof(char) + MAX_U64_SIZE + MAX_MD5_EXTRA_BYTES_NEEDED);
    if (to_hash_str == NULL) exit(EXIT_FAILURE);

    QUEUE_TYPE(uint16_t) queue = QUEUE_CREATE(uint16_t, 1000);

    uint16_t lookahead_counts[16] = {};
    for (size_t i = 1; i < 1000+1; i++) {
        memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
        sprintf(to_hash_str + strlen(SALT), "%zu", i);
    
        struct bits_128 hash = stretched_hash(to_hash_str);
        uint16_t bits = count_5kind_hashes(hash);
        QUEUE_PUSH(uint16_t, &queue, bits);
    }

    size_t hash_i = 0;
    size_t num_keys_found = 0;

    while (true) {
        memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
        sprintf(to_hash_str + strlen(SALT), "%zu", hash_i);

        if (hash_i % 100 == 0)
            printf("hash_i = %zu\n", hash_i);

        {
            struct bits_128 hash = stretched_hash(to_hash_str);
            int8_t triplet = first_triplet(hash);
            if (triplet != -1) {
                for (size_t i = 0; i < QUEUE_SIZE(uint16_t, &queue); i++) {
                    if ((QUEUE_PEEK(uint16_t, &queue, i) & (0x1 << triplet)) != 0) {
                        num_keys_found += 1;
                        break;
                    }
                }
            }

            if (num_keys_found == 64)
                break;

            hash_i += 1;
        }

        // remove last oldest item from the lookahead
        QUEUE_POP(uint16_t, &queue);

        {
            // add next item to the lookahead
            memcpy(to_hash_str, SALT, strlen(SALT) * sizeof(char));
            sprintf(to_hash_str + strlen(SALT), "%zu", hash_i + 1000);
            struct bits_128 hash = stretched_hash(to_hash_str);
            uint16_t bits = count_5kind_hashes(hash);
            QUEUE_PUSH(uint16_t, &queue, bits);
        }
    }

    // old method took 68s
    // new method takes ... 45s
    // TODO: how could we even hash faster using a single core?
    free(to_hash_str);
    printf("index of 64th key %zu\n", hash_i);
}

int main() {
    part1();
    part2();
    return EXIT_SUCCESS;
}