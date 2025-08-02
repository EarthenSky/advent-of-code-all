#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// #include <math.h>

#include "helpers.h"

void part1();
void part2();

int main() {
    char *file_contents = get_file_contents("input_day12.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

enum JSON_Kind {
    INT = 0,
    STRING,
    LIST,
    OBJECT
};
struct JSON_Int {
    enum JSON_Kind tag;
    int64_t value;
};
struct JSON_String {
    enum JSON_Kind tag;
    // TODO: actually store the string!
};
struct JSON_List {
    enum JSON_Kind tag;
    size_t number_of_items;
    union JSON_Item **items;
};
struct JSON_Object {
    enum JSON_Kind tag;
    //char **keys;
    size_t number_of_items;
    union JSON_Item **items;
};
union JSON_Item {
    enum JSON_Kind tag;
    struct JSON_Int json_int;
    struct JSON_String json_string;
    struct JSON_List json_list;
    struct JSON_Object json_object;
};

enum JumpTableState {
    EXPECTING_JSON_ITEM = 0,
    EXPECTING_JSON_ITEM_IN_LIST,
    EXPECTING_JSON_ITEM_IN_OBJ,
    EXPECTING_COMMA_OR_LIST_END,
    EXPECTING_COMMA_OR_OBJ_END,
    EXPECTING_OBJ_KEY_OR_END
};

// TODO: make this MUCH more DRY (without ruining performance!)

/// @brief will only accept valid json. Only operates on ASCII strings. Does not allow string to have internal whitespace
/// TODO: implement nicer error handling?
union JSON_Item *deserialize_json(const char *json_string) {
    enum JumpTableState state = EXPECTING_JSON_ITEM;
    // TODO: the initial NULL pointer for this is a bit awkward...
    union JSON_Item *current_item = NULL;

    const size_t JUMP_TABLE_SIZE = 255;
    uint8_t *jump_table = malloc(JUMP_TABLE_SIZE * sizeof(uint8_t));
    memset(jump_table, 0, JUMP_TABLE_SIZE * sizeof(uint8_t));

    const size_t BACKTRACK_TABLE_SIZE = 255;
    uint8_t *backtrack_table = malloc(BACKTRACK_TABLE_SIZE * sizeof(uint8_t));
    memset(backtrack_table, 0, BACKTRACK_TABLE_SIZE * sizeof(uint8_t));

    const size_t MAX_JSON_DEPTH = 1024;
    size_t size_past_state_list = 0;
    enum JumpTableState past_state_list[1024];
    union JSON_Item *past_json_items[1024];

    const size_t MAX_NUM_ITEMS_IN_LIST = 1024;
    const size_t MAX_NUM_ITEMS_IN_DICT = 1024;

    size_t json_i = 0;
    size_t json_string_len = strlen(json_string);
    
    while (json_i < json_string_len) {
        if (state == EXPECTING_JSON_ITEM || state == EXPECTING_JSON_ITEM_IN_LIST || state == EXPECTING_JSON_ITEM_IN_OBJ) {
            char next_char = json_string[json_i];
            // printf("next char: %c\n", json_string[json_i]);
            json_i += 1;

            switch (next_char) {
                case '[': {
                    union JSON_Item *item = malloc(sizeof(struct JSON_List));
                    item->tag = LIST;
                    item->json_list.number_of_items = 0;
                    item->json_list.items = malloc(MAX_NUM_ITEMS_IN_LIST * sizeof(union JSON_Item *));
                
                    if (state == EXPECTING_JSON_ITEM_IN_LIST)
                        past_state_list[size_past_state_list] = EXPECTING_COMMA_OR_LIST_END;
                    else if (state == EXPECTING_JSON_ITEM_IN_OBJ)
                        past_state_list[size_past_state_list] = EXPECTING_COMMA_OR_OBJ_END;
                    else
                        past_state_list[size_past_state_list] = EXPECTING_JSON_ITEM;
                    state = EXPECTING_JSON_ITEM_IN_LIST;
                    past_json_items[size_past_state_list] = current_item;
                    current_item = item;
                    
                    size_past_state_list += 1;
                    break;
                }
                case '{': {
                    union JSON_Item *item = malloc(sizeof(struct JSON_Object));
                    item->tag = OBJECT;
                    item->json_object.number_of_items = 0;
                    item->json_object.items = malloc(MAX_NUM_ITEMS_IN_DICT * sizeof(union JSON_Item *));
                    // TODO: actually store the keys!
                    // item->json_object.keys = malloc(MAX_NUM_ITEMS_IN_DICT * sizeof(char *));

                    if (state == EXPECTING_JSON_ITEM_IN_LIST)
                        past_state_list[size_past_state_list] = EXPECTING_COMMA_OR_LIST_END;
                    else if (state == EXPECTING_JSON_ITEM_IN_OBJ)
                        past_state_list[size_past_state_list] = EXPECTING_COMMA_OR_OBJ_END;
                    else
                        past_state_list[size_past_state_list] = EXPECTING_JSON_ITEM;
                    state = EXPECTING_OBJ_KEY_OR_END;
                    past_json_items[size_past_state_list] = current_item;
                    current_item = item;
                    
                    size_past_state_list += 1;
                    break;
                }
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
                    int64_t number;
                    int num_characters_scanned;
                    int result = sscanf(json_string+json_i-1, "%lld%n", &number, &num_characters_scanned);
                    if (result == -1) {
                        printf("ERROR: failed to scan integer\n");
                        exit(1);
                    }
                    json_i += num_characters_scanned - 1;

                    // TODO: are unions allowed to point at an object like this? I hope UB happens if we try to access
                    // one of the unknown items.
                    union JSON_Item *item = malloc(sizeof(struct JSON_Int));
                    item->tag = INT;
                    item->json_int.value = number;

                    if (state == EXPECTING_JSON_ITEM) {
                        return item;
                    } else if (state == EXPECTING_JSON_ITEM_IN_LIST) {
                        current_item->json_list.items[current_item->json_list.number_of_items] = item;
                        current_item->json_list.number_of_items += 1;
                        state = EXPECTING_COMMA_OR_LIST_END;
                    } else if (state == EXPECTING_JSON_ITEM_IN_OBJ) {
                        // b/c keys occur first
                        current_item->json_object.items[current_item->json_object.number_of_items] = item;
                        current_item->json_object.number_of_items += 1;
                        state = EXPECTING_COMMA_OR_OBJ_END;
                    }
                    break;
                }
                case '"': {
                    char string_buffer[128 + 1];
                    memset(string_buffer, 0, 128+1);

                    // TODO: enable parsing strings that contain spaces & quotes & stuff!
                    int num_characters_scanned;
                    int result = sscanf(json_string+json_i, "%128[^\"]\"%n", string_buffer, &num_characters_scanned);
                    if (result == -1) {
                        printf("ERROR (2): failed to scan integer\n");
                        exit(1);
                    } else if (num_characters_scanned != 1 + strlen(string_buffer)) {
                        // this ensures that the format string ends in a quote
                        // TODO: ensure the string does not end in an escaped quote
                        printf("ERROR: key was not formed properly (only scanned %d/%llu chars)\n", num_characters_scanned, 1+strlen(string_buffer));
                        exit(1);
                    }
                    json_i += num_characters_scanned;

                    union JSON_Item *item = malloc(sizeof(struct JSON_String));
                    item->tag = STRING;
                    // TODO: actually store the string

                    if (state == EXPECTING_JSON_ITEM) {
                        return item;
                    } else if (state == EXPECTING_JSON_ITEM_IN_LIST) {
                        current_item->json_list.items[current_item->json_list.number_of_items] = item;
                        current_item->json_list.number_of_items += 1;
                        state = EXPECTING_COMMA_OR_LIST_END;
                    } else if (state == EXPECTING_JSON_ITEM_IN_OBJ) {
                        // b/c keys occur first
                        current_item->json_object.items[current_item->json_object.number_of_items] = item;
                        current_item->json_object.number_of_items += 1;
                        state = EXPECTING_COMMA_OR_OBJ_END;
                    }
                    break;
                }
                // occurs via empty list
                case ']': {
                    enum JumpTableState past_state = past_state_list[size_past_state_list-1];
                    if (past_state == EXPECTING_JSON_ITEM) {
                        return current_item;
                    } else if (past_state == EXPECTING_COMMA_OR_LIST_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_list.items[past_item->json_list.number_of_items] = current_item;
                        past_item->json_list.number_of_items += 1;
                        current_item = past_item;
                    } else if (past_state == EXPECTING_COMMA_OR_OBJ_END) {
                        printf("ERROR: unexpected char ']' for object\n");
                        exit(1);
                    }

                    state = past_state;
                    size_past_state_list -= 1;
                    break;
                }
                case '}': {
                    if (state == EXPECTING_JSON_ITEM_IN_OBJ) {
                        printf("ERROR: object closed without providing an item to match the key\n");
                        exit(1);
                    }

                    enum JumpTableState past_state = past_state_list[size_past_state_list-1];
                    if (past_state == EXPECTING_JSON_ITEM) {
                        return current_item;
                    } else if (past_state == EXPECTING_COMMA_OR_LIST_END) {
                        printf("ERROR: unexpected char '}' for list\n");
                        exit(1);
                    } else if (past_state == EXPECTING_COMMA_OR_OBJ_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_object.items[past_item->json_object.number_of_items] = current_item;
                        past_item->json_object.number_of_items += 1;
                        current_item = past_item;
                    }

                    state = past_state;
                    size_past_state_list -= 1;
                    break;
                }
                default:
                    printf("ERROR: unexpected character '%c'\n", next_char);
                    exit(1);
            }
        } else if (state == EXPECTING_COMMA_OR_LIST_END) {
            char next_char = json_string[json_i];
            // printf("next char (2): %c\n", json_string[json_i]);
            json_i += 1;
            
            switch (next_char) {
                case ',':
                    state = EXPECTING_JSON_ITEM_IN_LIST;
                    break;

                case ']':
                    enum JumpTableState past_state = past_state_list[size_past_state_list-1];
                    if (past_state == EXPECTING_JSON_ITEM) {
                        return current_item;
                    } else if (past_state == EXPECTING_COMMA_OR_LIST_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_list.items[past_item->json_list.number_of_items] = current_item;
                        past_item->json_list.number_of_items += 1;
                        current_item = past_item;
                    } else if (past_state == EXPECTING_COMMA_OR_OBJ_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_object.items[past_item->json_object.number_of_items] = current_item;
                        past_item->json_object.number_of_items += 1;
                        current_item = past_item;
                    } else {
                        printf("unreachable (3)\n");
                        exit(1);
                    }

                    state = past_state;
                    size_past_state_list -= 1;
                    break;

                default:
                    printf("ERROR: unexpected character '%c' (in state %hhu)\n", next_char, state);
                    exit(1);
            }
        } else if (state == EXPECTING_OBJ_KEY_OR_END) {
            char next_char = json_string[json_i];
            // printf("next char (3): %c\n", json_string[json_i]);
            json_i += 1;
            
            switch (next_char) {
                case '"':
                    // TODO: enable parsing strings that contain spaces
                    char string_buffer[128 + 1];
                    memset(string_buffer, 0, 128+1);

                    int num_characters_scanned;
                    int result = sscanf(json_string+json_i, "%128[^\"]\":%n", string_buffer, &num_characters_scanned);
                    if (result == -1) {
                        printf("ERROR (2): failed to scan key\n");
                        exit(1);
                    } else if (num_characters_scanned != 2 + strlen(string_buffer)) {
                        // this ensures that the format string was fully matched against!
                        printf("ERROR: key was not formed properly (only scanned %d/%llu chars)\n", num_characters_scanned, 2+strlen(string_buffer));
                        exit(1);
                    }
                    json_i += num_characters_scanned;

                    state = EXPECTING_JSON_ITEM_IN_OBJ;
                    break;

                case '}':
                    enum JumpTableState past_state = past_state_list[size_past_state_list-1];
                    if (past_state == EXPECTING_JSON_ITEM) {
                        return current_item;
                    } else if (past_state == EXPECTING_COMMA_OR_LIST_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_list.items[past_item->json_list.number_of_items] = current_item;
                        past_item->json_list.number_of_items += 1;
                        current_item = past_item;
                    } else if (past_state == EXPECTING_COMMA_OR_OBJ_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_object.items[past_item->json_object.number_of_items] = current_item;
                        past_item->json_object.number_of_items += 1;
                        current_item = past_item;
                    } else {
                        printf("unreachable (3)\n");
                        exit(1);
                    }

                    state = past_state;
                    size_past_state_list -= 1;
                    break;

                default:
                    printf("ERROR: unexpected character '%c' (in state %hhu)\n", next_char, state);
                    exit(1);
            }

        } else if (state == EXPECTING_COMMA_OR_OBJ_END) {
            char next_char = json_string[json_i];
            // printf("next char (4): %c\n", json_string[json_i]);
            json_i += 1;
            
            switch (next_char) {
                case ',':
                    state = EXPECTING_OBJ_KEY_OR_END;
                    break;
                case '}':
                    enum JumpTableState past_state = past_state_list[size_past_state_list-1];
                    if (past_state == EXPECTING_JSON_ITEM) {
                        return current_item;
                    } else if (past_state == EXPECTING_COMMA_OR_LIST_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_list.items[past_item->json_list.number_of_items] = current_item;
                        past_item->json_list.number_of_items += 1;
                        current_item = past_item;
                    } else if (past_state == EXPECTING_COMMA_OR_OBJ_END) {
                        union JSON_Item *past_item = past_json_items[size_past_state_list-1];
                        past_item->json_object.items[past_item->json_object.number_of_items] = current_item;
                        past_item->json_object.number_of_items += 1;
                        current_item = past_item;
                    } else {
                        printf("unreachable (3)\n");
                        exit(1);
                    }

                    state = past_state;
                    size_past_state_list -= 1;
                    break;

                default:
                    printf("ERROR (2): unexpected character '%c' (in state %hhu)\n", next_char, state);
                    exit(1);
            }
        }
    }

    if (state == EXPECTING_JSON_ITEM && size_past_state_list == 0) {
        return current_item;
    } else {
        printf("ERROR: incomplete object\n");
        exit(2);
    }
}

// recursively!
void free_json(union JSON_Item *json_item) {
    // TODO: this
}

int64_t count_integer_leaves(union JSON_Item *json_item) {
    switch (json_item->tag) {
        case INT:
            // printf("-> int %llu\n", json_item->json_int.value);
            return json_item->json_int.value;
        case STRING:
            // printf("-> string %llu\n", 0);
            return 0;
        case LIST: {
            // printf("-> list...\n");
            int64_t sum = 0;
            for (size_t i = 0; i < json_item->json_list.number_of_items; i++) {
                sum += count_integer_leaves(json_item->json_list.items[i]);
            }
            return sum;
        }
        case OBJECT: {
            // printf("-> obj...\n");
            int64_t sum = 0;
            for (size_t i = 0; i < json_item->json_object.number_of_items; i++) {
                sum += count_integer_leaves(json_item->json_object.items[i]);
            }
            return sum;
        }
        default:
            printf("unreachable\n");
            exit(2);
    }
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        // NOTE: there's only a single token containing the entire json object. No spaces.
        union JSON_Item *json = deserialize_json(token);

        printf("sum_of_all_numbers: %lld\n", count_integer_leaves(json));
        free_json(json);
    }
    free(file_contents_copy);
}

void part2(const char *file_contents, size_t file_size) {
    printf("\npart2:\n");
}