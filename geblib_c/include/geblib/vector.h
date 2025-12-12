#ifndef GEBLIB_VECTOR_H
#define GEBLIB_VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_VECTOR(type) struct type ## _vector {\
    type *data;\
    size_t size;\
    size_t capacity;\
};\
struct type ## _vector type ## _vector__create(size_t initial_capacity) {\
    if (initial_capacity == 0) {\
        fprintf(stderr, "ERROR: cannot create empty vector\n");\
        exit(EXIT_FAILURE);\
    }\
    return (struct type ## _vector) {\
        .data = malloc(initial_capacity * sizeof(type)),\
        .size = 0,\
        .capacity = initial_capacity\
    };\
}\
void type ## _vector__add(struct type ## _vector *vec, type x) {\
    if (vec->size == vec->capacity) {\
        type *old = vec->data;\
        vec->data = realloc(vec->data, vec->capacity * 2 * sizeof(type));\
        if (vec->data == NULL) {\
            free(old);\
            fprintf(stderr, "ERROR: type_vector__add:malloc() failed");\
            exit(EXIT_FAILURE);\
        }\
        vec->capacity *= 2;\
    }\
\
    vec->data[vec->size] = x;\
    vec->size += 1;\
}\
type type ## _vector__pop(struct type ## _vector *vec) {\
    if (vec->size == 0) {\
        fprintf(stderr, "ERROR: cannot pop item from empty vector\n");\
        exit(EXIT_FAILURE);\
    }\
    type result = vec->data[vec->size-1];\
    vec->size -= 1;\
    return result;\
}\
void type ## _vector__free(struct type ## _vector *vec) {\
    if (vec->data != NULL)\
        free(vec->data);\
}\
void type ## _vector__insert(struct type ## _vector *vec, size_t i, type x) {\
    if (i > vec->size) {\
        fprintf(stderr, "ERROR: cannot insert item outside of vector\n");\
        exit(EXIT_FAILURE);\
    } else if (vec->size == vec->capacity) {\
        type *old = vec->data;\
        vec->data = realloc(vec->data, vec->capacity * 2 * sizeof(type));\
        if (vec->data == NULL) {\
            free(old);\
            fprintf(stderr, "ERROR: type_vector__add:malloc() failed");\
            exit(EXIT_FAILURE);\
        }\
        vec->capacity *= 2;\
    }\
    memmove(vec->data+i+1, vec->data+i, (vec->size - i) * sizeof(type));\
    vec->data[i] = x;\
    vec->size += 1;\
}

#define VECTOR_TYPE(type) struct type ## _vector
#define VECTOR_CREATE(type, initial_capacity) type ## _vector__create(initial_capacity)
#define VECTOR_ADD(type, vec, x) type ## _vector__add(vec, x)
#define VECTOR_POP(type, vec) type ## _vector__pop(vec)
#define VECTOR_FREE(type, vec) type ## _vector__free(vec)
#define VECTOR_INSERT(type, vec, i, x) typ ## e_vector__insert(vec, i, x)

#endif