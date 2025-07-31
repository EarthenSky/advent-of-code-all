#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void both_parts();

int main() {
    const char *file_contents = "hxbxwxba";
    size_t file_size = strlen(file_contents);
    both_parts(file_contents, file_size);
    return 0;
}

// ----------------------------------------------------------------------- //

void increment_string(char *string) {
    for (size_t i = strlen(string)-1; i >= 0; i--) {
        if (string[i] == 'z') {
            string[i] = 'a';
        } else {
            string[i] += 1;
            return;
        }
    }
}
bool satisfies_rules(const char *password) {
    size_t password_len = strlen(password);
    bool contains_len_3_straight = false;
    size_t num_duplicate_pairs = 0;
    int64_t last_num_duplicate_pair = -1;
    
    for (size_t i = 0; i < password_len; i++) {
        if (i >= 2)
            if (password[i-2] == (password[i-1] - 1) && (password[i-1]-1) == (password[i]-2))
                contains_len_3_straight = true;

        // greedy approach, so we will always get the maximum or equal to maximum number of non-overlapping matches
        // ex: aaaa -> a[aa]a (only 1)
        // ex: aaaa -> [aa][aa] (found 2!)
        if (i >= (last_num_duplicate_pair+2))
            if (password[i-1] == password[i]) {
                num_duplicate_pairs += 1;
                last_num_duplicate_pair = i;
            }
            
        if (password[i] == 'i' || password[i] == 'o' || password[i] == 'l')
            return false;
    }

    return (num_duplicate_pairs >= 2) && contains_len_3_straight;
}

void both_parts(const char *file_contents, size_t file_size) {
    char *password = malloc(file_size+1);
    memcpy(password, file_contents, file_size);
    password[file_size] = '\0';

    while (!satisfies_rules(password)) {
        increment_string(password);
    }
    
    printf("part1 password:\t%s\n", password);

    increment_string(password);
    while (!satisfies_rules(password)) {
        increment_string(password);
    }
    
    printf("part2 password:\t%s\n", password);
    free(password);
}