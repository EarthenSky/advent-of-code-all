#ifndef GEBLIB_MD5_H
#define GEBLIB_MD5_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

const size_t MAX_MD5_EXTRA_BYTES_NEEDED = 64+8;

struct bits_128 {
    uint8_t bytes[16];
};

uint32_t leftrotate(uint32_t x, uint32_t rotate_amount) {
    rotate_amount %= 32;
    return (x << rotate_amount) + (x >> (32 - rotate_amount));
}

/// @brief this algorithm is derived from https://en.wikipedia.org/wiki/MD5
struct bits_128 md5_inplace(char* message) {
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
    message[message_size + 0] = 0x80;
    memset(message + 1 + message_size, 0x00, padding_size * sizeof(char));
    uint64_t *bits = (uint64_t *) &message[message_size + 1 + padding_size];
    *bits = message_size * 8;

    // DEBUG:
    //for (size_t ci = 0; ci < new_message_size; ci++) {
    //    printf("[%llu] %c (%hhu)\n", ci, message[ci], message[ci]);
    //}

    // process the message in successive 512-bit chunks
    for (size_t message_i = 0; message_i < new_message_size; message_i += 64) {
        // NOTE: since message_i increments in steps of 64 and message is 4 byte aligned,
        // M will always be 4-byte aligned.
        uint32_t *M = (uint32_t *) (message + message_i);

        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;

        uint32_t F, g;
        for (size_t i = 0; i < 16; i++) {
            F = ((B & C) | (~B & D)) + A + K[i] + M[i];
            A = D;
            D = C;
            C = B;
            B += leftrotate(F, s[i]);
        }
        for (size_t i = 16; i < 32; i++) {
            F = ((D & B) | (~D & C)) + A + K[i] + M[(5*i + 1) % 16];
            A = D;
            D = C;
            C = B;
            B += leftrotate(F, s[i]);
        }
        for (size_t i = 32; i < 48; i++) {
            F = (B ^ C ^ D) + A + K[i] + M[(3*i + 5) % 16];
            A = D;
            D = C;
            C = B;
            B += leftrotate(F, s[i]);
        }
        for (size_t i = 48; i < 64; i++) {
            F = (C ^ (B | ~D)) + A + K[i] + M[(7*i) % 16];
            A = D;
            D = C;
            C = B;
            B += leftrotate(F, s[i]);
        }

        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

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

#endif