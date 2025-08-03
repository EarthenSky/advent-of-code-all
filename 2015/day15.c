#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <math.h>

#include "helpers.h"

void both_parts(const char *file_contents, size_t file_size);

// Measure-Command { ./build/Debug/day15.exe }
int main() {
    char *file_contents = get_file_contents("input_day15.txt");
    size_t file_size = strlen(file_contents);
    both_parts(file_contents, file_size);
    free(file_contents);
    return 0;
}

// ----------------------------------------------------------------------- //

struct ScoreProperties {
    int64_t capacity;
    int64_t durability;
    int64_t flavor;
    int64_t texture;
};

struct IngredientProperties {
    int64_t capacity;
    int64_t durability;
    int64_t flavor;
    int64_t texture;
    int64_t calories;
};

int64_t compute_best_score(
    struct IngredientProperties *ingredients,
    struct ScoreProperties score_properties,
    size_t num_ingredient_left,
    size_t num_tbsp_left
) {
    if (num_ingredient_left == 0 || num_tbsp_left == 0) {
        return (
              max(score_properties.capacity, 0)
            * max(score_properties.durability, 0)
            * max(score_properties.flavor, 0)
            * max(score_properties.texture, 0)
        );
    } else if (num_ingredient_left == 1) {
        return compute_best_score(
            ingredients,
            (struct ScoreProperties) {
                score_properties.capacity   + ingredients[0].capacity * num_tbsp_left,
                score_properties.durability + ingredients[0].durability * num_tbsp_left,
                score_properties.flavor     + ingredients[0].flavor * num_tbsp_left,
                score_properties.texture    + ingredients[0].texture * num_tbsp_left
            },
            0, 0
        );
    }

    int64_t best_score = 0;
    for (size_t i = 0; i <= num_tbsp_left; i++) {
        best_score = max(
            best_score,
            compute_best_score(
                ingredients+1,
                (struct ScoreProperties) {
                    score_properties.capacity   + ingredients[0].capacity * i,
                    score_properties.durability + ingredients[0].durability * i,
                    score_properties.flavor     + ingredients[0].flavor * i,
                    score_properties.texture    + ingredients[0].texture * i
                },
                num_ingredient_left - 1,
                num_tbsp_left - i
            )
        );
    }
    return best_score;
}

int64_t compute_best_score_500_calories(
    struct IngredientProperties *ingredients,
    struct IngredientProperties properties,
    size_t num_ingredient_left,
    size_t num_tbsp_left
) {
    if (num_ingredient_left == 0 || num_tbsp_left == 0) {
        if (properties.calories != 500)
            return 0;
        else
            return (
                max(properties.capacity, 0)
                * max(properties.durability, 0)
                * max(properties.flavor, 0)
                * max(properties.texture, 0)
            );
    } else if (num_ingredient_left == 1) {
        return compute_best_score_500_calories(
            ingredients,
            (struct IngredientProperties) {
                properties.capacity   + ingredients[0].capacity * num_tbsp_left,
                properties.durability + ingredients[0].durability * num_tbsp_left,
                properties.flavor     + ingredients[0].flavor * num_tbsp_left,
                properties.texture    + ingredients[0].texture * num_tbsp_left,
                properties.calories   + ingredients[0].calories * num_tbsp_left
            },
            0, 0
        );
    } else if (properties.calories >= 500) {
        // a lil constraint-based optimization! (b/c calories are always positive)
        return 0;
    }

    int64_t best_score = 0;
    for (size_t i = 0; i <= num_tbsp_left; i++) {
        best_score = max(
            best_score,
            compute_best_score_500_calories(
                ingredients+1,
                (struct IngredientProperties) {
                    properties.capacity   + ingredients[0].capacity * i,
                    properties.durability + ingredients[0].durability * i,
                    properties.flavor     + ingredients[0].flavor * i,
                    properties.texture    + ingredients[0].texture * i,
                    properties.calories   + ingredients[0].calories * i
                },
                num_ingredient_left - 1,
                num_tbsp_left - i
            )
        );
    }
    return best_score;
}

// kinda like a multi-dimensional knapsack problem, except that all items are the
// same size and the capacity is the product of . Not exactly sure what problem this is most similar to?
void both_parts(const char *file_contents, size_t file_size) {
    printf("part1:\n");

    size_t num_ingredients = 0;
    struct IngredientProperties ingredients[4];

    // make copy for strtok to mangle (null terminators)
    char *file_contents_copy = (char *) malloc(file_size+1);
    memcpy(file_contents_copy, file_contents, file_size+1);
    for (char *token = strtok(file_contents_copy, "\n"); token != NULL; token = strtok(NULL, "\n")) {
        int result = sscanf(
            token, "%*s capacity %lld, durability %lld, flavor %lld, texture %lld, calories %lld",
            &ingredients[num_ingredients].capacity,
            &ingredients[num_ingredients].durability,
            &ingredients[num_ingredients].flavor,
            &ingredients[num_ingredients].texture,
            &ingredients[num_ingredients].calories
        ); 
        if (result != 5) {
            printf("ERROR: failed to parse line (got %d)\n", result);
            exit(1);
        }
        num_ingredients += 1; 
    }
    free(file_contents_copy);

    int64_t best_score = compute_best_score(ingredients, (struct ScoreProperties) { 0, 0, 0, 0 }, 4, 100);
    printf("best_score: %lld\n", best_score);

    printf("\npart2:\n");
    int64_t best_score_500_calories = compute_best_score_500_calories(
        ingredients, (struct IngredientProperties) { 0, 0, 0, 0, 0 }, 4, 100
    );
    printf("best_score_500_calories: %lld\n", best_score_500_calories);
}
