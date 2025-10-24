#ifndef GEBLIB_HASHSET_H
#define GEBLIB_HASHSET_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_HASHSET(type) struct type_hashset__item {\
    bool exists;\
    type value;\
};\
struct type_hashset {\
    struct type_hashset__item *data;\
    size_t size;\
    size_t capacity;\
    size_t (*hash_function)(const type*);\
    bool (*equality_function)(const type*, const type*);\
};\
struct type_hashset type_hashset__create(\
    size_t initial_capacity,\
    size_t (*hash_function)(const type*),\
    bool (*equality_function)(const type*, const type*)\
) {\
    if (initial_capacity == 0) {\
        fprintf(stderr, "ERROR: cannot create empty hashset\n");\
        exit(EXIT_FAILURE);\
    }\
    return (struct type_hashset) {\
        .data = malloc(initial_capacity * sizeof(struct type_hashset__item)),\
        .size = initial_capacity,\
        .capacity = initial_capacity,\
        .hash_function = hash_function,\
        .equality_function = equality_function\
    };\
}\
size_t type_hashset__follow_chain(const struct type_hashset *hs, const type *x) {\
    size_t hash = hs->hash_function(x) % hs->capacity;\
    size_t offset = 0;\
    while (true) {\
        struct type_hashset__item item = hs->data[(hash + offset) % hs->capacity];\
        if (!item.exists)\
            break;\
        else if (hs->equality_function(x, &item.value))\
            break;\
\
        offset += 1;\
    }\
    return hash + offset;\
}\
/* returns true if inserted a new element, false if it already existed. */\
bool type_hashset__insert(struct type_hashset *hs, const type *x) {\
    /* will resize when 50% full, to ensure thread sizes are smallish */\
    /* TODO: control over this factor would be a nice touch */\
    if (hs->size * 2 >= hs->capacity) {\
        struct type_hashset__item *new_data = calloc(\
            hs->capacity * 2, sizeof(struct type_hashset__item)\
        );\
        if (new_data == NULL) {\
            free(hs->data);\
            fprintf(stderr, "ERROR: type_hashset__insert:malloc() failed");\
            exit(EXIT_FAILURE);\
        }\
\
        struct type_hashset new_hashset = (struct type_hashset) {\
            .data = new_data,\
            .size = hs->size,\
            .capacity = hs->capacity * 2,\
            .hash_function = hs->hash_function,\
            .equality_function = hs->equality_function\
        };\
        for (size_t i = 0; i < hs->capacity; i++) {\
            if (hs->data[i].exists) {\
                size_t hash_i = type_hashset__follow_chain(&new_hashset, &hs->data[i].value);\
                new_hashset.data[hash_i] = hs->data[i];\
            }\
        }\
        free(hs->data);\
        hs->data = new_hashset.data;\
        hs->capacity = new_hashset.capacity;\
    }\
\
    size_t hash_i = type_hashset__follow_chain(hs, x);\
    if (hs->data[hash_i].exists)\
        return false;\
\
    hs->data[hash_i] = (struct type_hashset__item) {\
        .exists = true,\
        .value = *x\
    };\
    hs->size += 1;\
    return true;\
}\
bool type_hashset__contains(struct type_hashset *hs, const type *x) {\
    size_t hash_i = type_hashset__follow_chain(hs, x);\
    return hs->data[hash_i].exists;\
}\
void type_hashset__free(struct type_hashset *hs) {\
    if (hs->data != NULL)\
        free(hs->data);\
}

#define HASHSET_TYPE(type) struct type_hashset
#define HASHSET_CREATE(type, initial_capacity, hash_function, equality_function) type_hashset__create(initial_capacity, hash_function, equality_function)
#define HASHSET_INSERT(type, hs, x) type_hashset__insert(hs, x)
#define HASHSET_CONTAINS(type, hs, x) type_hashset__contains(hs, x)
#define HASHSET_FREE(type, hs) type_hashset__free(hs)

#endif