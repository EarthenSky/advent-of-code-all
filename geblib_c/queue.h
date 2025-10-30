#ifndef GEBLIB_QUEUE_H
#define GEBLIB_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_QUEUE(type) struct type_queue {\
    type *data;\
    size_t start_i;\
    size_t end_i;\
    size_t capacity;\
};\
struct type_queue type_queue__create(size_t initial_capacity) {\
    if (initial_capacity == 0) {\
        fprintf(stderr, "ERROR: cannot create empty queue\n");\
        exit(EXIT_FAILURE);\
    }\
    return (struct type_queue) {\
        .data = malloc(initial_capacity * sizeof(type)),\
        .start_i = 0,\
        .end_i = 0,\
        .capacity = initial_capacity\
    };\
}\
void type_queue__push(struct type_queue *q, type x) {\
    if (q->start_i == (q->end_i + 1) % q->capacity) {\
        type *old = q->data;\
        q->data = realloc(q->data, q->capacity * 2 * sizeof(type));\
        if (q->data == NULL) {\
            free(old);\
            fprintf(stderr, "ERROR: type_queue__push:realloc() failed");\
            exit(EXIT_FAILURE);\
        }\
        if (q->end_i < q->start_i) {\
            memcpy(q->data + q->capacity, q->data, q->end_i * sizeof(type));\
            q->end_i = q->capacity + q->end_i;\
        }\
        q->capacity *= 2;\
    }\
\
    q->data[q->end_i] = x;\
    q->end_i = (q->end_i + 1) % q->capacity;\
}\
type type_queue__pop(struct type_queue *q) {\
    if (q->start_i == q->end_i) {\
        fprintf(stderr, "ERROR: cannot pop item from empty queue\n");\
        exit(EXIT_FAILURE);\
    }\
    type result = q->data[q->start_i];\
    q->start_i = (q->start_i + 1) % q->capacity;\
    return result;\
}\
type type_queue__peek(const struct type_queue *q, size_t i) {\
    size_t target_i = (q->start_i + i) % q->capacity;\
    if (\
        q->start_i <= q->end_i\
        && (target_i < q->start_i || target_i >= q->end_i)\
    ) {\
        fprintf(stderr, "ERROR: index outside range\n");\
        exit(EXIT_FAILURE);\
    } else if (\
        q->start_i > q->end_i\
        && target_i < q->start_i\
        && target_i >= q->end_i\
    ) {\
        fprintf(stderr, "ERROR: index outside range\n");\
        exit(EXIT_FAILURE);\
    }\
    return q->data[target_i];\
}\
void type_queue__free(struct type_queue *q) {\
    if (q->data != NULL)\
        free(q->data);\
}\
size_t type_queue__size(const struct type_queue *q) {\
    if (q->end_i >= q->start_i)\
        return q->end_i - q->start_i;\
    else\
        return (q->end_i + q->capacity) - q->start_i;\
}

#define QUEUE_TYPE(type) struct type_queue
#define QUEUE_CREATE(type, initial_capacity) type_queue__create(initial_capacity)
// TODO: make this a pointer to be consistent w/ the other ADTs
#define QUEUE_PUSH(type, q, x) type_queue__push(q, x)
#define QUEUE_POP(type, q) type_queue__pop(q)
#define QUEUE_PEEK(type, q, i) type_queue__peek(q, i)
#define QUEUE_FREE(type, q) type_queue__free(q)
#define QUEUE_SIZE(type, q) type_queue__size(q)

#endif