#include <assert.h>
#include <stdint.h>
#include <stdlib.h> // free(), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>  // sscanf() ?
#include <stdbool.h> // true / false
#include <string.h> // strlen()

// sys means POSIX here
#include <sys/types.h> // ssize_t

void part1();
void part2();

int main() {
    part1();
    part2();
    return EXIT_SUCCESS;
}

// returns success
bool translate_button(char ch, int8_t *current_button) {
    switch (ch) {
        case 'U':
            if (*current_button > 3)
                *current_button -= 3;
            return true;
        case 'D':
            if (*current_button <= 6)
                *current_button += 3;
            return true;
        case 'L':
            if ((*current_button % 3) != 1)
                *current_button -= 1;
            return true;
        case 'R':
            if ((*current_button % 3) != 0)
                *current_button += 1;
            return true;
        default:
            fprintf(stderr, "ERROR: got unknown char %c (%d) in line\n", ch, ch);
            return false;
    }
}

#define MAX_PASSWORD_SIZE (32)

void part1() {
    printf("part 1:\n");

    FILE *f = fopen("input_day2.txt", "r");
    if (f == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    uint8_t password[MAX_PASSWORD_SIZE];
    size_t password_size = 0;

    uint8_t current_button = 5;

    char *line = NULL;
    size_t line_buffer_size = 0;
    // TODO: oops, this is part of a C extension!
    ssize_t line_size = 0;
    while (true) {
        // TODO: oops, this is part of a C extension!
        line_size = getline(&line, &line_buffer_size, f);
        if (line == NULL) {
            fprintf(stderr, "ERROR: getline() alloc failed");

            free(line);
            line = NULL;
            return;
        } else if (line_size <= 0) {
            free(line);
            line = NULL;
            break;
        }

        // ignore newline at end of line
        for (size_t i = 0; i < line_size; i++) {
            if (line[i] == '\n') {
                free(line);
                line = NULL;
                continue;
            } else if (!translate_button(line[i], &current_button)) {
                free(line);
                line = NULL;
                return;
            }
        }

        password[password_size] = current_button;
        password_size += 1;

        free(line);
        line = NULL;
    }

    printf("password = ");
    for (size_t i = 0; i < password_size; i++)
        printf("%d", password[i]);
    printf("\n");
}

uint8_t get_row(char button) {
    if (button == '1') {
        return 0;
    } else if (button >= '1' && button <= '4') {
        return 1;
    } else if (button >= '5' && button <= '9') {
        return 2;
    } else if (button >= 'A' && button <= 'C') {
        return 3;
    } else if (button == 'D') {
        return 4;
    } else {
        return 255;
    }
}
uint8_t get_column(char button) {
    if (button == '5') {
        return 0;
    } else if (button == '2' || button == '6' || button == 'A') {
        return 1;
    } else if (button == '1' || button == '3' || button == '7' || button == 'B' || button == 'D') {
        return 2;
    } else if (button == '4' || button == '8' || button == 'C') {
        return 3;
    } else if (button == '9') {
        return 4;
    } else {
        return 255;
    }
}

bool translate_button_2(char ch, char *current_button) {
    uint8_t row = get_row(*current_button);
    uint8_t column = get_column(*current_button);
    switch (ch) {
        case 'U':
            if (*current_button == '3')
                *current_button -= 2;
            else if (row == 2 && column > 0 && column < 4)
                *current_button -= 4;
            else if (row == 3)
                *current_button -= 4;
            else if (row == 4)
                *current_button -= 2;

            // move from the letters to the digits
            if (*current_button < 'A' && *current_button > '9')
                *current_button = *current_button - 'A' + 1 + '9';

            return true;
        case 'D':
            if (*current_button == 'B')
                *current_button += 2;
            else if (row == 2 && column > 0 && column < 4)
                *current_button += 4;
            else if (row == 1)
                *current_button += 4;
            else if (row == 0)
                *current_button += 2;

            // move from the digits to the letters
            if (*current_button > '9' && *current_button < 'A')
                *current_button = *current_button - '9' - 1 + 'A';

            return true;
        case 'L':
            if (
                *current_button != '1'
                && *current_button != '2'
                && *current_button != '5'
                && *current_button != 'A'
                && *current_button != 'D'
            )
                *current_button -= 1;
            return true;
        case 'R':
            if (
                *current_button != '1'
                && *current_button != '4'
                && *current_button != '9'
                && *current_button != 'C'
                && *current_button != 'D'
            )
                *current_button += 1;
            return true;
        default:
            fprintf(stderr, "ERROR: got unknown char %c (%d) in line\n", ch, ch);
            return false;
    }
}

void part2() {
    printf("part2:\n");

    FILE *f = fopen("input_day2.txt", "r");
    if (f == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    char password[MAX_PASSWORD_SIZE];
    size_t password_size = 0;

    char current_button = '5';

    char *line = NULL;
    size_t line_buffer_size = 0;
    ssize_t line_size = 0;
    while (true) {
        line_size = getline(&line, &line_buffer_size, f);
        if (line == NULL) {
            fprintf(stderr, "ERROR: getline() alloc failed");

            free(line);
            line = NULL;
            return;
        } else if (line_size <= 0) {
            free(line);
            line = NULL;
            break;
        }

        // ignore newline at end of line
        for (size_t i = 0; i < line_size; i++) {
            if (line[i] == '\n') {
                free(line);
                line = NULL;
                continue;
            }
            
            if (!translate_button_2(line[i], &current_button)) {
                free(line);
                line = NULL;
                return;
            }
        }

        password[password_size] = current_button;
        password_size += 1;

        free(line);
        line = NULL;
    }

    printf("password = ");
    for (size_t i = 0; i < password_size; i++)
        printf("%c", password[i]);
    printf("\n");
}