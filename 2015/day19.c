#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <ctype.h>
// #include <math.h>

#include "helpers.h"

void part1(const char *file_contents, size_t file_size);
void part2(const char *file_contents, size_t file_size);

// Measure-Command { ./build/Debug/day19.exe }
int main() {
    char *file_contents = get_file_contents("input_day19.txt");
    size_t file_size = strlen(file_contents);
    part1(file_contents, file_size);
    part2(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

// TODO: it'd be nice if we could make this binary search
bool molecule_list_contains(
    char *molecule_list,
    size_t num_molecules,
    char *target_molecule
) {
    for (size_t i = 0; i < num_molecules; i++) {
        if (strncmp(&molecule_list[i * 512], target_molecule, 512) == 0) {
            return true;
        }
    }
    return false;
}

char new_molecule_buffer[512];

void all_replacements(
    char *molecule_list,
    size_t *num_molecules,
    const char *molecule,
    const char *rule_source,
    const char *rule_targets
) {
    size_t molecule_len = strlen(molecule);
    size_t rule_source_len = strlen(rule_source);
    size_t rule_targets_len = strlen(rule_targets);

    // printf("RULE %s -> %s\n", rule_source, rule_targets);

    for (size_t i = 0; i < molecule_len; i++) {
        if (i >= rule_source_len-1) {
            size_t element_start_i = i - (rule_source_len-1);
            if (strncmp(rule_source, molecule+element_start_i, rule_source_len) == 0) {
                size_t new_molecule_size = molecule_len + rule_targets_len - rule_source_len;

                memcpy(new_molecule_buffer, molecule, element_start_i);
                memcpy(new_molecule_buffer + element_start_i, rule_targets, rule_targets_len);
                memcpy(
                    new_molecule_buffer + element_start_i + rule_targets_len,
                    molecule + element_start_i + rule_source_len,
                    molecule_len - (element_start_i + rule_source_len)
                );
                new_molecule_buffer[new_molecule_size] = '\0';

                // add new molecule if it is unique
                if (!molecule_list_contains(molecule_list, *num_molecules, new_molecule_buffer)) {
                    memcpy(&molecule_list[*num_molecules * 512], new_molecule_buffer, new_molecule_size+1);
                    *num_molecules += 1;
                }
            }
        }
    }
}

void part1(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    size_t num_rules = 0;
    char rule_source[64][2+1];
    char rule_targets[64][32+1];

    char medicine_molecule[512];

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        if (strstr(token, " => ") == NULL) {
            int result = sscanf(token, "%512s", medicine_molecule);
            if (result != 1) {
                printf("ERROR: failed to parse rule\n");
                exit(1);
            }
        } else {
            // process rule
            int result = sscanf(token, "%2s => %32s", rule_source[num_rules], rule_targets[num_rules]);
            if (result != 2) {
                printf("ERROR: failed to parse rule\n");
                exit(1);
            }

            num_rules += 1;
        }
    }
    free(file_contents_copy);

    {
        size_t num_unique_molecules = 0;
        char *molecule_list = malloc(2048 * 512 * sizeof(char));
        for (size_t i = 0; i < num_rules; i++) {
            all_replacements(molecule_list, &num_unique_molecules, medicine_molecule, rule_source[i], rule_targets[i]);
        }
        free(molecule_list);
        printf("num_unique_molecules: %llu\n", num_unique_molecules);
    }
}

/// @brief -1 means false
int token_list_index(
    char *token_list,
    size_t num_tokens,
    char *target_token
) {
    for (size_t i = 0; i < num_tokens; i++) {
        if (strncmp(&token_list[i * 3], target_token, 3) == 0) {
            return i;
        }
    }
    return -1;
}

// TODO: tokenize everything
void followset(
    
) {

}

void part2(const char *file_contents, size_t file_size) {
    // index in this list is the token's value
    size_t num_tokens = 0;
    char token_list[64 * 3];

    size_t num_rules = 0;
    uint8_t rule_source[64];
    uint8_t num_targets[64];
    uint8_t rule_targets[64][16];

    size_t size_of_medicine_molecule = 0;
    uint8_t medicine_molecule[512];

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        if (strstr(token, " => ") == NULL) {
            // TODO: tokenize
            int result = sscanf(token, "%512s", medicine_molecule);
            if (result != 1) {
                printf("ERROR: failed to parse rule\n");
                exit(1);
            }
        } else {
            // process rule
            char rule_source[2+1];
            char rule_targets[32+1];
            int result = sscanf(token, "%2s => %32s", rule_source, rule_targets);
            if (result != 2) {
                printf("ERROR: failed to parse rule\n");
                exit(1);
            }

            // get token from the rule source
            int token_i_of_source = token_list_index(token_list, num_tokens, rule_source);
            if (token_i_of_source == -1) {
                memcpy(&token_list[num_tokens * 3], rule_source, 3);
                token_i_of_source = num_tokens;
                num_tokens += 1;
            }
            rule_source[num_rules] = token_i_of_source; 

            // get tokens from rule target (must be of the form upper_case_letter lowercase_letter, or just upper_case_letter)
            num_targets[num_rules] = 0;
            for (size_t i = 0; i < strlen(rule_targets); i++) {
                if (i >= 1) {
                    if (isupper(rule_targets[i-1]) && isupper(rule_targets[i])) {
                        char new_token[2] = { rule_targets[i-1], '\0' };
                        int token_i = token_list_index(token_list, num_tokens, new_token);
                        if (token_i == -1) {
                            memcpy(&token_list[num_tokens * 3], new_token, 2);
                            token_i = num_tokens;
                            num_tokens += 1;
                        }
                        rule_targets[num_targets[num_rules]] = token_i; 
                        num_targets[num_rules] += 1;

                    } else if (islower(rule_targets[i-1]) && isupper(rule_targets[i]) && i == (strlen(rule_targets)-1)) {
                        char new_token[2] = { rule_targets[i], '\0' };
                        int token_i = token_list_index(token_list, num_tokens, new_token);
                        if (token_i == -1) {
                            memcpy(&token_list[num_tokens * 3], new_token, 2);
                            token_i = num_tokens;
                            num_tokens += 1;
                        }
                        rule_targets[num_targets[num_rules]] = token_i; 
                        num_targets[num_rules] += 1;

                    } else if (isupper(rule_targets[i-1]) && islower(rule_targets[i])) {
                        char new_token[3] = { rule_targets[i-1], rule_targets[i], '\0' };
                        int token_i = token_list_index(token_list, num_tokens, new_token);
                        if (token_i == -1) {
                            memcpy(&token_list[num_tokens * 3], new_token, 3);
                            token_i = num_tokens;
                            num_tokens += 1;
                        }
                        rule_targets[num_targets[num_rules]] = token_i; 
                        num_targets[num_rules] += 1;

                    }
                }
            }

            num_rules += 1;
        }
    }
    free(file_contents_copy);

    // TODO: print out all the rules to make sure they're being parsed correctly

    // TODO: compute follow & pre-sets

    // TODO: prune & see if it improves performance
        
    // ----- molecule fabrication ----- //
    // now we need to perform UCS to find our target molecule
    // maybe even A* if the search is slow

    // SOLUTION
    // treat this as a grammar; computing the closure / followset or something should allow us to work backwards & determine
    // how the molecule was formed?
    // Yeah, we do bottom up parsing & figure out all the possible ways that the molecule could have been generated, then
    // select the smallest one.

    // IDEA 2: just do the same as part1, but backwards!

    printf("\npart2:\n");

    // start w/ e
    size_t num_unique_molecules = 0;
    char *molecule_list = malloc(32 * 2048 * 512 * sizeof(char));
    
    size_t last_num_unique_molecules = 1;
    char *last_molecule_list = malloc(32 * 2048 * 512 * sizeof(char));
    memcpy(last_molecule_list, medicine_molecule, strlen(medicine_molecule));
    last_molecule_list[strlen(medicine_molecule)] = '\0';

    size_t smallest_molecule = 10000;

    for (size_t step_i = 0; step_i < 5; step_i++) {
        printf("(%llu) last_num_unique_molecules: %llu\n", step_i, last_num_unique_molecules);

        for (size_t mol_i = 0; mol_i < last_num_unique_molecules; mol_i++) {
            char *mol = &last_molecule_list[mol_i * 512];
            for (size_t rule_i = 0; rule_i < num_rules; rule_i++) {
                if (strlen(mol) < smallest_molecule) {
                    smallest_molecule = strlen(mol);
                    printf(">> mol %s (%llu)\n", mol, smallest_molecule);
                }
                // TODO: update this to use tokens instead of strings
                all_replacements(
                    molecule_list, &num_unique_molecules,
                    mol,
                    rule_targets[rule_i],
                    rule_source[rule_i]
                );
            }
            // printf("\tnum_unique_molecules %llu\n", num_unique_molecules);
        }

        // swap buffers, reset the new buffer to store more molecules
        last_num_unique_molecules = num_unique_molecules;
        num_unique_molecules = 0;

        char* tmp = last_molecule_list;
        last_molecule_list = molecule_list;
        molecule_list = tmp;
    }

    free(molecule_list);
    free(last_molecule_list);

    printf("completed\n");
}