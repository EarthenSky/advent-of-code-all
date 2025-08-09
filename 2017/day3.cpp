#include "helpers.h"

// #include <algorithm> // std::count
#include <cmath> // std::sqrt, std::max? 
#include <iostream> // std::cout
#include <fstream> // std::ifstream, std::ios
#include <filesystem> // TODO: what does std::filesystem do specially?
#include <format> // std::format
// #include <ranges> // std::views::split
#include <string> // size_t, std::stoi

#include <unordered_map>

void part1(size_t number) {
    // 1, 6+2, (2*5)+(2*3), 14+10, 18+14
    // 1, 9, 25, 49, 81
    // 1, 3, 5, 7, 9
    size_t ring_larger = (size_t) std::ceil(std::sqrt(number));
    ring_larger = ring_larger % 2 == 0 ? ring_larger + 1 : ring_larger;
    size_t ring_smaller = (size_t) std::floor(std::sqrt(number-1));
    ring_smaller = ring_smaller % 2 == 0 ? ring_smaller - 1 : ring_smaller;

    // each ring has size 4n-4, so we can solve for n
    // n = 0, 2, 4, 6, 8
    size_t ring_n = ring_larger - 1;
    size_t ring_i = number - ring_smaller*ring_smaller - 1;
    size_t orthogonal_component = (size_t) abs(
        (ring_n/2 - 1) - (ring_i % ring_n)
    );

    size_t number_of_steps = orthogonal_component + ring_n/2;
    std::cout << std::format("(part1) number_of_steps: {}", number_of_steps) << std::endl;
}

struct HashPair {
    size_t operator()(const std::pair<int32_t,int32_t>& pair) const noexcept {
        return pair.first * 100000 + pair.second;
    }
};

typedef std::unordered_map<std::pair<int32_t, int32_t>, size_t, HashPair> xy_hashmap;

// this is the lame approach, wheras if we used some integer logic, we could break it into all possible cases & improve
// performance A LOT!
// However, the logic has a lot of corner cases and sounds painful, so we're skipping it for now :3
size_t stress_test_value(xy_hashmap& cached_values, size_t x, size_t y) {
    size_t sum = 0;
    if (cached_values.contains(std::pair(x+1, y)))
        sum += cached_values[std::pair(x+1, y)];
    if (cached_values.contains(std::pair(x-1, y)))
        sum += cached_values[std::pair(x-1, y)];

    if (cached_values.contains(std::pair(x+1, y+1)))
        sum += cached_values[std::pair(x+1, y+1)];
    if (cached_values.contains(std::pair(x, y+1)))
        sum += cached_values[std::pair(x, y+1)];
    if (cached_values.contains(std::pair(x-1, y+1)))
        sum += cached_values[std::pair(x-1, y+1)];

    if (cached_values.contains(std::pair(x+1, y-1)))
        sum += cached_values[std::pair(x+1, y-1)];
    if (cached_values.contains(std::pair(x, y-1)))
        sum += cached_values[std::pair(x, y-1)];
    if (cached_values.contains(std::pair(x-1, y-1)))
        sum += cached_values[std::pair(x-1, y-1)];

    cached_values[std::pair(x, y)] = sum;
    return sum;
}

void part2(size_t number) {

    // NOTE: skipping this approach!
    // 
    // rules for the funky recurrence relation!
    // a(i) = current row + next row + last row
    // current row = (a(i-1) + 0 + 0) or (a(i-2) + a(i-1) + 0 + 0) [just after corners]
    // next row = (0 + 0 + 0) or (0 + 0 + 0 + 0 + 0) [larger on corners]
    // last row = corner on corner, double corner one after, three matching on not corner. -1 when approaching corner, repeat.

    xy_hashmap cached_values;
    cached_values[std::pair(0, 0)] = 1;

    size_t i = 1;
    size_t value = 1;
    while (value <= number) {
        i += 1;

        size_t ring_larger = (size_t) std::ceil(std::sqrt(i));
        ring_larger = ring_larger % 2 == 0 ? ring_larger + 1 : ring_larger;
        // this -1 fixes floor not working due to being exactly on the edge of the square!
        size_t ring_smaller = (size_t) std::floor(std::sqrt(i-1));
        ring_smaller = ring_smaller % 2 == 0 ? ring_smaller - 1 : ring_smaller;

        size_t ring_n = ring_larger - 1;
        size_t ring_i = i - ring_smaller*ring_smaller - 1;
        size_t orthogonal_component = (size_t) (
            (ring_i % ring_n) - (ring_n/2 - 1)
        );
        
        int32_t x, y;
        size_t quadrant = (ring_i % (ring_n*4)) / ring_n;
        std::cout << std::format("(part2) quadrant: {}, ri: {}", quadrant, ring_i) << std::endl;
        if (quadrant == 0) {
            x = ring_n/2;
            y = orthogonal_component;
        } else if (quadrant == 1) {
            x = -orthogonal_component;
            y = ring_n/2;
        } else if (quadrant == 2) {
            x = -ring_n/2;
            y = -orthogonal_component;
        } else if (quadrant == 3) {
            x = orthogonal_component;
            y = -ring_n/2;
        }
        value = stress_test_value(cached_values, x, y);
        //std::cout << std::format("(part2) i, value: {}, {}", i, value) << std::endl;
    }

    std::cout << std::format("(part2) first_larger_value: {}", value) << std::endl;
}

int main() {
    std::filesystem::path file_path = "day3_input.txt";
    std::ifstream input_file(file_path);

    std::basic_string<char> number_string;
    std::getline(input_file, number_string);
    size_t number = (size_t) std::stoi(number_string);

    part1(number);
    part2(number);
    return 0;
}