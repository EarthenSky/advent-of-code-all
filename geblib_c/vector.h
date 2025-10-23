#ifndef GEBLIB_VECTOR_H
#define GEBLIB_VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: also make it generic to type using macros
struct char_vector {
    char *data;
    size_t size;
    size_t capacity;
};
struct char_vector char_vector__create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        fprintf(stderr, "ERROR: cannot create empty vector\n");
        exit(EXIT_FAILURE);
    }
    return (struct char_vector) {
        .data = malloc(initial_capacity * sizeof(char)),
        .size = 0,
        .capacity = initial_capacity
    };
}
void char_vector__add(struct char_vector vec, char ch) {
    if (vec.size == vec.capacity) {
        vec.data = realloc(vec.data, vec.capacity * 2);
        vec.capacity *= 2;
        if (vec.data == NULL) {
            fprintf(stderr, "ERROR: char_vector__add:malloc() failed");
            exit(EXIT_FAILURE);
        }
    }

    vec.data[vec.size] = ch;
    vec.size += 1;
}
void char_vector__free(struct char_vector vec) {
    free(vec.data);
}

#endif