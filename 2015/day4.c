#include <assert.h>
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
    const char *file_contents = "yzbqklnj";
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    return 0;
}

// ----------------------------------------------------------------------- //

// https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function
inline unsigned int leftrotate(const uint32_t value, uint32_t shift) {
    if ((shift &= sizeof(value)*8 - 1) == 0)
        return value;
    return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

const uint32_t s[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};
const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// Implementation & constants come from https://en.wikipedia.org/wiki/MD5#Algorithm
// we can do just a single pass, because our data is small enough!
void md5_hash(char* secret, uint32_t output[4]) {
    // NOTE: this assumes that the secret is never larger than 31 bytes (reasonable?)
    char input[64];
    memset(input, 0, 64);
    size_t secret_size = strlen(secret);
    memcpy(input, secret, secret_size);
    input[secret_size] = 0x80;
    // NOTE: only taking up the lsb of the 64 bit int representing size in bits,
    // since we can never be larger than 32 bytes. lsb is the left most in little endian. 
    // This can do up to secret size 31, b/c 32 * 8 == 256 > 255
    input[63-7] = (uint8_t) (secret_size * 8);

    /*printf("input: ");
    for (size_t i = 0; i < 64; i++) {
        printf("%02hhx ", input[i]);
    }
    printf("\n");*/

    uint32_t M[16];
    memcpy(M, input, 64);

    output[0] = 0x67452301;
    output[1] = 0xefcdab89;
    output[2] = 0x98badcfe;
    output[3] = 0x10325476;

    uint32_t A = output[0];
    uint32_t B = output[1];
    uint32_t C = output[2];
    uint32_t D = output[3];

    for (uint32_t i = 0; i <= 15; i++) {
        uint32_t F = (D ^ (B & (C ^ D))) + A + K[i] + M[i]; // M[i] must be a 32-bit block
        A = D;
        D = C;
        C = B;
        B = B + leftrotate(F, s[i]);
    }
    for (uint32_t i = 16; i <= 31; i++) {
        uint32_t F = (C ^ (D & (B ^ C))) + A + K[i] + M[(5*i+1) % 16];
        A = D;
        D = C;
        C = B;
        B = B + leftrotate(F, s[i]);
    }
    for (uint32_t i = 32; i <= 47; i++) {
        uint32_t F = (B ^ C ^ D) + A + K[i] + M[(3*i+5) % 16];
        A = D;
        D = C;
        C = B;
        B = B + leftrotate(F, s[i]);
    }
    for (uint32_t i = 48; i <= 63; i++) {
        uint32_t F = (C ^ (B | ~D)) + A + K[i] + M[(7*i) % 16];
        A = D;
        D = C;
        C = B;
        B = B + leftrotate(F, s[i]);
    }

    output[0] += A;
    output[1] += B;
    output[2] += C;
    output[3] += D;
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    size_t i = 1;
    while (true) {
        char input_str[32];
        memset(input_str, 0, 32);
        assert(strlen(file_contents) == 8);
        memcpy(input_str, file_contents, strlen(file_contents));

        sprintf(input_str + strlen(file_contents), "%llu", i);
        assert(strlen(file_contents) <= 31);

        char result[16];
        md5_hash(input_str, (uint32_t *) result);

        /*printf("bytes: ");
        for (size_t i = 0; i < 16; i++) {
            printf("%02hhx ", result[i]);
        }
        printf("\n");*/

        if (result[0] == 0 && result[1] == 0 && (result[2] & 0xf0) == 0) { // && (result[2] & 0xf0) == 0) {
            printf("smallest number: %llu\n", i);
            return;
        }

        i++;
    }
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");

    char input_str[32];
    memset(input_str, 0, 32);
    memcpy(input_str, file_contents, file_size);

    uint32_t i = 0;
    sprintf(input_str + file_size, "%u", i);
    uint64_t index_last_char = strlen(input_str) - 1;

    while (true) {
        i++;

        if (i % 10 == 0) {
            // only calls sprintf 10% of the time it usually would, as it's
            // surprisingly slow!
            sprintf(input_str + file_size, "%u", i);
            index_last_char = strlen(input_str) - 1;
        } else {
            input_str[index_last_char] = '0' + (i % 10);
        }
        // assert(strlen(input_str) <= 31);

        char result[16];
        md5_hash(input_str, (uint32_t *) result);

        if (result[0] == 0 && result[1] == 0 && (result[2] & 0xff) == 0) { 
            printf("smallest number: %u\n", i);
            return;
        }
    }
}