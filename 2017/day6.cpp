#include <array>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <format>
#include <functional>
#include <numeric>
#include <string>

#include "file_input.h"

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

        std::vector<std::array<uint8_t, 16>> history;

        // add initial state
        std::array<uint8_t, 16> state;
        std::ranges::copy(int_list.begin(), int_list.end(), state.begin());
        history.push_back(state);

        size_t redist_i;
        bool found_duplicate = false;
        for (redist_i = 0; found_duplicate == false; redist_i++) {
            this->redistribution_cycle(state);

            // linear search (boo)
            found_duplicate = std::ranges::find(history, state) != history.end();
            history.push_back(state);
        }

        std::cout << std::format("redist_i: {}", redist_i) << std::endl;

    }

    void part2(const std::vector<uint8_t>& int_list) const {
        std::vector<std::array<uint8_t, 16>> history;

        // add initial state
        std::array<uint8_t, 16> state;
        std::copy(int_list.begin(), int_list.end(), state.begin());
        history.push_back(state);

        bool found_duplicate = false;
        for (size_t redist_i = 0; found_duplicate == false; redist_i++) {
            this->redistribution_cycle(state);

            // linear search (boo)
            found_duplicate = std::ranges::find(history, state) != history.end();
            if (found_duplicate) {
                size_t target_i = std::ranges::find(history, state) - history.begin();
                std::cout << std::format("loop_size: {}", (redist_i+1) - target_i) << std::endl;
            }
            history.push_back(state);
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