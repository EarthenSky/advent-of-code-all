#include <array>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <format>
#include <functional>
#include <numeric>
#include <string>
#include <unordered_set>

#include "file_input.h"

struct ArrayHash {
    size_t operator()(const std::array<uint8_t, 16>& array) const noexcept {
        size_t tmp_hash = 0;
        for (size_t i = 0; i < array.size(); i++) {
            tmp_hash = (103 * tmp_hash) ^ std::hash<uint8_t>{}(array[i]);
        }
        return tmp_hash;
    }
};

class Solution {
public:
    void redistribution_cycle(std::array<uint8_t, 16>& memory_banks) const {
        size_t largest_bank_i = 0;
        for (size_t i = 0; i < 16; i++) {
            // ties won by lowest numbered bank!
            if (memory_banks[i] > memory_banks[largest_bank_i]) {
                largest_bank_i = i;
            }
        }

        size_t size_of_largest_bank = memory_banks[largest_bank_i];
        memory_banks[largest_bank_i] = 0;

        for (size_t i = 0; i < size_of_largest_bank; i++) {
            memory_banks[(largest_bank_i + 1 + i) % 16] += 1;
        }
    }

    void part1(const std::vector<uint8_t>& int_list) const {
        // size_t sum = std::accumulate(int_list.begin(), int_list.end(), 0, std::plus<>());
        // fits in u8!

        std::unordered_set<std::array<uint8_t, 16>, ArrayHash> history;

        // add initial state
        std::array<uint8_t, 16> state;
        std::ranges::copy(int_list.begin(), int_list.end(), state.begin());
        history.insert(state);

        size_t redist_i;
        bool found_duplicate = false;
        for (redist_i = 0; found_duplicate == false; redist_i++) {
            this->redistribution_cycle(state);

            // hash lookup; woo!
            found_duplicate = history.contains(state);
            history.insert(state);
        }

        std::cout << std::format("redist_i: {}", redist_i) << std::endl;
    }

    void part2(const std::vector<uint8_t>& int_list) const {
        std::unordered_map<std::array<uint8_t, 16>, size_t, ArrayHash> history;

        // add initial state
        std::array<uint8_t, 16> state;
        std::copy(int_list.begin(), int_list.end(), state.begin());
        history.insert(std::make_pair(state, 0));

        bool found_duplicate = false;
        for (size_t redist_i = 0; found_duplicate == false; redist_i++) {
            this->redistribution_cycle(state);

            // linear search (boo)
            found_duplicate = history.contains(state);
            if (found_duplicate) {
                size_t target_i = history[state];
                std::cout << std::format("loop_size: {}", (redist_i+1) - target_i) << std::endl;
            }
            history.insert(std::make_pair(state, redist_i));
        }

    }
};

int main() {
    InputFile input_file(std::filesystem::path("./day6_input.txt"));
    std::vector<uint8_t> int_list = input_file.parse_day6();

    Solution solution;
    solution.part1(int_list);
    solution.part2(int_list);
    return 0;
}