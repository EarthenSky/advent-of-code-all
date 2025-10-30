#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../geblib_c/io.h"

struct argument {
    // anonymous enum is in the file scope because structs don't have scope
    enum {
        REG = 0,
        VALUE
    } tag;
    // anonymous union to the rescue!
    union {
        uint8_t reg_i;
        int64_t value;
    };
};

enum instruction_tag {
    CPY = 0,
    INC,
    DEC,
    JNZ
};
typedef struct instruction {
    enum instruction_tag tag;
    union {
        // TODO: do we even need the cpy, inc, dec, etc... names?
        struct { struct argument a; struct argument b; } cpy;
        struct { uint8_t reg_i; } inc;
        struct { uint8_t reg_i; } dec;
        struct { struct argument a; struct argument b; } jnz;
    };
} instruction_t;

// none of our input integers are larger than this
#define MAX_INT_SIZE 10

struct argument arg_from_str(const char *s) {
    if (s[0] >= 'a' && s[0] <= 'd') {
        return (struct argument) {
            .tag = REG,
            .reg_i = s[0] - 'a'
        };
    } else {
        return (struct argument) {
            .tag = VALUE,
            .value = atoi(s) // stdlib.h
        };
    }
}

/// @param reg out
void evaluate_program(int64_t reg[4], instruction_t instructions[], size_t num_instructions) {
    int64_t iptr = 0;

    while (iptr >= 0 && iptr < num_instructions) {
        instruction_t current = instructions[iptr];
        switch (current.tag) {
            case CPY:
                if (current.cpy.a.tag == REG)
                    reg[current.cpy.b.reg_i] = reg[current.cpy.a.reg_i];
                else
                    reg[current.cpy.b.reg_i] = current.cpy.a.value;
                iptr += 1;
                continue;
            case INC:
                reg[current.inc.reg_i] += 1;
                iptr += 1;
                continue;
            case DEC:
                reg[current.dec.reg_i] -= 1;
                iptr += 1;
                continue;
            case JNZ:
                bool a_is_zero = (
                    current.cpy.a.tag == REG
                    ? reg[current.jnz.a.reg_i]
                    : current.jnz.a.value
                ) == 0;
                if (!a_is_zero && current.cpy.b.tag == REG)
                    iptr += reg[current.cpy.b.reg_i];
                else if (!a_is_zero && current.cpy.b.tag == VALUE)
                    iptr += current.cpy.b.value;
                else
                    iptr += 1;
                continue;
        }
    }

}

void both_parts(const struct file_info* fi) {
    printf("part 1:\n");

    size_t num_instructions = 0;
    instruction_t instructions[256];

    size_t ch_i = 0;
    while (true) {
        size_t tokens_consumed;
        if (strncmp("cpy ", fi->file_bytes + ch_i, 4) == 0) {
            char a[MAX_INT_SIZE], b[MAX_INT_SIZE];
            if (sscanf(fi->file_bytes + ch_i, "cpy %[a-z0-9-] %[a-z0-9-] %zn", &a, &b, &tokens_consumed) != 2) {
                fprintf(stderr, "ERROR: unexpected num captures (cpy)\n");
                exit(EXIT_FAILURE);
            }

            instructions[num_instructions] = (instruction_t) {
                .tag = CPY,
                .cpy = { .a = arg_from_str(a), .b = arg_from_str(b) }
            };

        } else if (strncmp("inc ", fi->file_bytes + ch_i, 4) == 0) {
            char a;
            if (sscanf(fi->file_bytes + ch_i, "inc %c %zn", &a, &tokens_consumed) != 1) {
                fprintf(stderr, "ERROR: unexpected num captures (inc)\n");
                exit(EXIT_FAILURE);
            }

            instructions[num_instructions] = (instruction_t) {
                .tag = INC,
                .inc = { .reg_i = a - 'a' }
            };

        } else if (strncmp("dec ", fi->file_bytes + ch_i, 4) == 0) {
            char a;
            if (sscanf(fi->file_bytes + ch_i, "dec %c %zn", &a, &tokens_consumed) != 1) {
                fprintf(stderr, "ERROR: unexpected num captures (dec)\n");
                exit(EXIT_FAILURE);
            }

            instructions[num_instructions] = (instruction_t) {
                .tag = DEC,
                .dec = { .reg_i = a - 'a' }
            };

        } else if (strncmp("jnz ", fi->file_bytes + ch_i, 4) == 0) {
            char a[MAX_INT_SIZE], b[MAX_INT_SIZE];
            if (sscanf(fi->file_bytes + ch_i, "jnz %[a-z0-9-] %[a-z0-9-] %zn", &a, &b, &tokens_consumed) != 2) {
                fprintf(stderr, "ERROR: unexpected num captures (jnz)\n");
                exit(EXIT_FAILURE);
            }

            instructions[num_instructions] = (instruction_t) {
                .tag = JNZ,
                .jnz = { .a = arg_from_str(a), .b = arg_from_str(b) }
            };

        } else {
            break;
        }

        // each line parses and adds one instruction to our program
        num_instructions += 1;
        ch_i += tokens_consumed;
    }

    int64_t reg[4] = {};
    evaluate_program(reg, instructions, num_instructions);
    printf("a contains %lld\n", reg[0]);
    
    printf("part 2:\n");

    memcpy(reg, (int64_t[4]) { 0, 0, 1, 0 }, sizeof reg);
    evaluate_program(reg, instructions, num_instructions);

    printf("a contains %lld\n", reg[0]);
}

int main() {
    struct file_info fi = get_file_contents("./input_day12.txt");
    both_parts(&fi);

    free_file_info(fi);
    return EXIT_SUCCESS;
}