#include <stdio.h> // sprintf()
#include <stdlib.h> // exit()
#include <string.h>
#include <stdint.h>

const char* DOOR_ID = "ffykfhsq";

struct bits_128 {
    uint8_t bytes[16];
};

uint32_t leftrotate(uint32_t x, uint32_t rotate_amount) {
    rotate_amount = rotate_amount % 32;
    return (x << rotate_amount) + (x >> (32 - rotate_amount));
}

/// TODO: add support for malloc failure
/// @brief this algorithm is derived from https://en.wikipedia.org/wiki/MD5
struct bits_128 md5(const char* message) {
    uint32_t s[64] = {
        // per-round shifts
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };
    uint32_t K[64] = {
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
    uint32_t i;

    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    size_t message_size = strlen(message);
    // add the byte 0x80, pad with zeros until 448, then add the length
    size_t expanded_message_size = message_size + 1 + 8;
    size_t padding_size = 64 - expanded_message_size % 64;
    size_t new_message_size = expanded_message_size + padding_size;
    uint8_t* new_message = malloc(new_message_size * sizeof(char));
    // NOTE: new_message is not null terminated
    // TODO: can these fail?
    memcpy(new_message, message, message_size);
    new_message[message_size + 0] = 0x80;
    // TODO: give this a constant
    memset(new_message + 1 + message_size, 0x00, padding_size * sizeof(char));
    uint64_t *bits = (uint64_t *) &new_message[message_size + 1 + padding_size];
    *bits = message_size * 8;

    //for (size_t ci = 0; ci < new_message_size; ci++) {
    //    printf("[%llu] %c (%u)\n", ci, new_message[ci], new_message[ci]);
    //}

    // process the message in successive 512-bit chunks
    for (size_t message_i = 0; message_i < new_message_size; message_i += 64) {
        // TODO: why cast here or in malloc?
        uint32_t *M = (uint32_t *) &new_message[message_i];

        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;

        // Main loop:
        for (size_t i = 0; i < 64; i++) {
            uint32_t F, g;
            if (i >= 0 && i < 16) {
                F = (B & C) | (~B & D);
                g = i;
            } else if (16 <= i && i < 32) {
                F = (D & B) | (~D & C);
                g = (5*i + 1) % 16;
            } else if (32 <= i && i < 48) {
                F = B ^ C ^ D;
                g = (3*i + 5) % 16;
            } else if (48 <= i && i < 64) {
                F = C ^ (B | ~D);
                g = (7*i) % 16;
            }

            F = F + A + K[i] + M[g];
            A = D;
            D = C;
            C = B;

            B = B + leftrotate(F, s[i]);
        }

        // Add this chunk's hash to the result so far:
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    free(new_message);

    return (struct bits_128) {                                                           
        .bytes = {
            *(((char *)&a0)+0), *(((char *)&a0)+1),
            *(((char *)&a0)+2), *(((char *)&a0)+3),

            *(((char *)&b0)+0), *(((char *)&b0)+1),
            *(((char *)&b0)+2), *(((char *)&b0)+3),

            *(((char *)&c0)+0), *(((char *)&c0)+1),
            *(((char *)&c0)+2), *(((char *)&c0)+3),

            *(((char *)&d0)+0), *(((char *)&d0)+1),
            *(((char *)&d0)+2), *(((char *)&d0)+3),
        }
    };
}

void part1() {
    printf("part 1:\n");

    size_t password_i = 0;
    // TODO: how does this initialization work, formally speaking?
    char password[8+1] = { '\0' };

    const size_t MAX_CHARS_IN_U64 = 20;
    char *to_hash_buffer = malloc((strlen(DOOR_ID) + MAX_CHARS_IN_U64) * sizeof(char));
    if (to_hash_buffer == NULL) {
        printf("ERROR: malloc failed\n");
        exit(1);
    }

    // small test for md5!
    // printf("md5() = %x\n", md5("The quick brown fox jumps over the lazy dog").bytes[0]);
    // exit(2);

    size_t i = 0;
    while (password_i < 8) {
        sprintf(to_hash_buffer, "%s%llu", DOOR_ID, i);
        struct bits_128 hash = md5(to_hash_buffer);

        // if hash matches 0x00000x... add x to the password 
        if (
            hash.bytes[0] == 0x00
            && hash.bytes[1] == 0x00
            && (hash.bytes[2] & 0xf0) == 0x00
        ) {
            uint8_t hex_digit = hash.bytes[2] & 0x0f;
            if (hex_digit <= 9) {
                password[password_i] = '0' + hex_digit;
            } else {
                password[password_i] = 'a' + (hex_digit - 10);
            }
            password_i += 1;
        }

        i += 1;
    }

    // != f65a1172
    free(to_hash_buffer);
    printf("password: %s\n", password);
}

void part2() {
    printf("part 2:\n");
}

int main() {
    part1();
    part2();
    return 0;
}