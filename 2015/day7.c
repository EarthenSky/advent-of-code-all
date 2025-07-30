#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "helpers.h"

void find_wire_a(const char *file_contents, size_t file_size);

// cmake --build build; ./build/Debug/day7.exe
int main() {
    char *file_contents = get_file_contents("input_day7.txt");
    size_t file_size = strlen(file_contents);
    find_wire_a(file_contents, file_size);
    free(file_contents);

    // TODO: instead of just updating the file manually, write some code to do
    // this nicely
    // It's a lame solution, but I'm getting sleepy...
    file_contents = get_file_contents("input_day7_manually_overridden.txt");
    find_wire_a(file_contents, file_size);
    free(file_contents);

    return 0;
}

// ----------------------------------------------------------------------- //
// baby hashmap impl, but with strings now!

enum ValueKind {
    VALUE_WIRE = 0,
    VALUE_SIGNAL
};
struct ValueWire {
    enum ValueKind tag;
    char name[3];
};
struct ValueSignal {
    enum ValueKind tag;
    uint16_t value;
};
union Value {
    enum ValueKind tag;
    struct ValueWire wire;
    struct ValueSignal signal;
};

enum GateKind {
    AND = 0,
    OR,
    RSHIFT,
    LSHIFT,
    NOT,
    WIRE
};

struct AndGate {
    enum GateKind tag;
    union Value operand_a;
    union Value operand_b;
};
struct OrGate {
    enum GateKind tag;
    union Value operand_a;
    union Value operand_b;
};
struct LShiftGate {
    enum GateKind tag;
    union Value operand;
    uint8_t amount;
};
struct RShiftGate {
    enum GateKind tag;
    union Value operand;
    uint8_t amount;
};
struct NotGate {
    enum GateKind tag;
    union Value operand;
};
struct Wire {
    enum GateKind tag;
    union Value value;
};

union Gate {
    enum GateKind tag;
    struct AndGate and_gate;
    struct OrGate or_gate;
    struct LShiftGate l_shift_gate;
    struct RShiftGate r_shift_gate;
    struct NotGate not_gate;
    struct Wire wire;
};


const int32_t NOT_CACHED = -1;
struct HashTableEntry {
    bool is_set;
    char key[3];
    union Gate gate;
    int32_t cached_gate_value;
};

uint64_t hash_together(uint64_t x, uint64_t y) {
    return (uint64_t) (x * 37) ^ (uint64_t) (y * 103101);
}

// return the first x,y match or unset item
int32_t get_hash_table_index_of(
    const struct HashTableEntry *hash_table,
    const uint32_t hash_table_size,
    const char* key
) {
    uint64_t key_hash = 0;
    size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; i++) {
        key_hash = hash_together(key_hash, key[i]);
    }

    size_t original_index = key_hash % hash_table_size;
    size_t index = original_index;
    struct HashTableEntry entry = hash_table[index];
    if (!entry.is_set) {
        return index;
    } else {
        while (strcmp(key, entry.key) != 0) {
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

uint16_t get_value_of_signal(const char* key, struct HashTableEntry *hash_table, size_t hash_table_size) {
    int32_t i = get_hash_table_index_of(hash_table, hash_table_size, key);
    struct HashTableEntry entry = hash_table[i];
    if (!entry.is_set) {
        printf("ERROR: got invalid entry with key='%s'\n", entry.key);
        exit(3);
    }

    if (entry.cached_gate_value != NOT_CACHED) {
        // printf("\t %2s using cached value: %d\n", key, entry.cached_gate_value);
        return entry.cached_gate_value;
    } else {
        uint16_t result;

        uint16_t left_value;
        uint16_t right_value;
        union Value operand_a;
        union Value operand_b;
        switch (entry.gate.tag) {
            case AND:
                operand_a = entry.gate.and_gate.operand_a;
                if (operand_a.tag == VALUE_WIRE) {
                    left_value = get_value_of_signal(operand_a.wire.name, hash_table, hash_table_size);
                } else {
                    left_value = operand_a.signal.value;
                }

                operand_b = entry.gate.and_gate.operand_b;
                if (operand_b.tag == VALUE_WIRE) {
                    right_value = get_value_of_signal(operand_b.wire.name, hash_table, hash_table_size);
                } else {
                    right_value = operand_b.signal.value;
                }

                result = left_value & right_value;
                break;
            case OR:
                operand_a = entry.gate.or_gate.operand_a;
                if (operand_a.tag == VALUE_WIRE) {
                    left_value = get_value_of_signal(operand_a.wire.name, hash_table, hash_table_size);
                } else {
                    left_value = operand_a.signal.value;
                }

                operand_b = entry.gate.or_gate.operand_b;
                if (operand_b.tag == VALUE_WIRE) {
                    right_value = get_value_of_signal(operand_b.wire.name, hash_table, hash_table_size);
                } else {
                    right_value = operand_b.signal.value;
                }

                result = left_value | right_value;
                break;
            case RSHIFT:
                if (entry.gate.r_shift_gate.operand.tag == VALUE_WIRE) {
                    result = (
                        get_value_of_signal(entry.gate.r_shift_gate.operand.wire.name, hash_table, hash_table_size)
                        >> entry.gate.r_shift_gate.amount
                    );
                } else {
                    result = entry.gate.r_shift_gate.operand.signal.value >> entry.gate.r_shift_gate.amount;
                }
                break;
            case LSHIFT:
                if (entry.gate.l_shift_gate.operand.tag == VALUE_WIRE) {
                    result = (
                        get_value_of_signal(entry.gate.l_shift_gate.operand.wire.name, hash_table, hash_table_size)
                        << entry.gate.l_shift_gate.amount
                    );
                } else {
                    result = entry.gate.l_shift_gate.operand.signal.value << entry.gate.l_shift_gate.amount;
                }
                break;
            case NOT:
                if (entry.gate.not_gate.operand.tag == VALUE_WIRE) {
                    result = ~get_value_of_signal(entry.gate.not_gate.operand.wire.name, hash_table, hash_table_size);
                } else {
                    result = ~entry.gate.not_gate.operand.signal.value;
                }
                break;
            case WIRE:
                if (entry.gate.wire.value.tag == VALUE_WIRE) {
                    result = get_value_of_signal(entry.gate.wire.value.wire.name, hash_table, hash_table_size);
                } else {
                    result = entry.gate.wire.value.signal.value;
                }
                break;
            default:
                printf("ERROR: unexpected gate tag");
                exit(2);
        }

        hash_table[i].cached_gate_value = result;
        return result;
    }
}

// determines whether it's an int value or a string value
union Value get_value_of_string(char a[6]) {
    union Value operand_a;
    if (a[0] >= '0' && a[0] <= '9') {
        operand_a.tag = VALUE_SIGNAL;
        sscanf(a, "%hu", &operand_a.signal.value);
    } else {
        operand_a.tag = VALUE_WIRE;
        memcpy(operand_a.wire.name, a, 3);
    }
    return operand_a;
}

void find_wire_a(const char *file_contents, size_t file_size) {
    printf("part1 (and 2):\n");

    // make a copy for strtok to mangle
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);

    // TODO: make a tree, then recursively compute the value of a.
    const size_t HASH_TABLE_SIZE = 500 * 4;
    struct HashTableEntry *hash_table = malloc(HASH_TABLE_SIZE * sizeof(struct HashTableEntry));
    memset(hash_table, 0, HASH_TABLE_SIZE * sizeof(struct HashTableEntry));

    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        //printf("token=%s\n", token);
        if (strstr(token, "AND") != NULL) {
            char a[6], b[6];
            char c[3];
            sscanf(token, "%5s AND %5s -> %2s", a, b, c);

            union Gate gate;
            gate.and_gate.tag = AND;
            gate.and_gate.operand_a = get_value_of_string(a);
            gate.and_gate.operand_b = get_value_of_string(b);

            struct HashTableEntry entry;
            entry.is_set = true;
            memcpy(entry.key, c, 3);
            entry.gate = gate;
            entry.cached_gate_value = NOT_CACHED;

            // assume no duplicates on rhs
            int32_t i = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, c);
            hash_table[i] = entry;

        } else if (strstr(token, "OR") != NULL) {
            char a[6], b[6];
            char c[3];
            sscanf(token, "%5s OR %5s -> %2s", a, b, c);

            union Gate gate;
            gate.or_gate.tag = OR;
            gate.or_gate.operand_a = get_value_of_string(a);
            gate.or_gate.operand_b = get_value_of_string(b);

            struct HashTableEntry entry;
            entry.is_set = true;
            memcpy(entry.key, c, 3);
            entry.gate = gate;
            entry.cached_gate_value = NOT_CACHED;

            // assume no duplicates on rhs
            int32_t i = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, c);
            hash_table[i] = entry;

        } else if (strstr(token, "LSHIFT") != NULL) {
            char a[6], c[3];
            uint8_t offset;
            sscanf(token, "%5s LSHIFT %hhu -> %2s", a, &offset, c);

            union Gate gate;
            gate.l_shift_gate.tag = LSHIFT;
            gate.l_shift_gate.operand = get_value_of_string(a);
            gate.l_shift_gate.amount = offset;

            struct HashTableEntry entry;
            entry.is_set = true;
            memcpy(entry.key, c, 3);
            entry.gate = gate;
            entry.cached_gate_value = NOT_CACHED;

            // assume no duplicates on rhs
            int32_t i = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, c);
            hash_table[i] = entry;

        } else if (strstr(token, "RSHIFT") != NULL) {
            char a[6], c[3];
            uint8_t offset;
            sscanf(token, "%5s RSHIFT %hhu -> %2s", a, &offset, c);

            union Gate gate;
            gate.r_shift_gate.tag = RSHIFT;
            gate.r_shift_gate.operand = get_value_of_string(a);
            gate.r_shift_gate.amount = offset;

            struct HashTableEntry entry;
            entry.is_set = true;
            memcpy(entry.key, c, 3);
            entry.gate = gate;
            entry.cached_gate_value = NOT_CACHED;

            // assume no duplicates on rhs
            int32_t i = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, c);
            hash_table[i] = entry;

        } else if (strstr(token, "NOT") != NULL) {
            char a[6], c[3];
            sscanf(token, "NOT %5s -> %2s", a, c);

            union Gate gate;
            gate.not_gate.tag = NOT;
            gate.not_gate.operand = get_value_of_string(a);

            struct HashTableEntry entry;
            entry.is_set = true;
            memcpy(entry.key, c, 3);
            entry.gate = gate;
            entry.cached_gate_value = NOT_CACHED;

            // assume no duplicates on rhs
            int32_t i = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, c);
            hash_table[i] = entry;

        } else {
            char a[6], c[3];
            sscanf(token, "%5s -> %2s", a, c);

            union Gate gate;
            gate.wire.tag = WIRE;
            gate.wire.value = get_value_of_string(a);

            struct HashTableEntry entry;
            entry.is_set = true;
            memcpy(entry.key, c, 3);
            entry.gate = gate;
            entry.cached_gate_value = NOT_CACHED;

            // assume no duplicates on rhs
            int32_t i = get_hash_table_index_of(hash_table, HASH_TABLE_SIZE, c);
            hash_table[i] = entry;
        }
    }

    printf("finding signal...\n");

    uint16_t value = get_value_of_signal("a", hash_table, HASH_TABLE_SIZE);
    printf("wire a: %hu\n", value);
}
