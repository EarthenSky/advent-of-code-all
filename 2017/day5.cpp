#include <iostream>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>

#include "file_input.h"

class Solution {
public:
    void part1(const std::vector<int64_t>& original_int_list) const {
        std::vector<int64_t> int_list = original_int_list;

        size_t step_i = 0;
        int64_t instr_ptr = 0;
        while (instr_ptr >= 0 && instr_ptr < int_list.size()) {
            int_list[instr_ptr] += 1;
            instr_ptr += int_list[instr_ptr] - 1;
            step_i += 1;
        }

        std::cout << std::format("(part1) step_i: {}", step_i) << std::endl;
    }

    void part2(const std::vector<int64_t>& original_int_list) const {
        std::vector<int64_t> int_list = original_int_list;

        size_t step_i = 0;
        int64_t instr_ptr = 0;
        while (instr_ptr >= 0 && instr_ptr < int_list.size()) {
            int64_t jump_amount = int_list[instr_ptr];
            int_list[instr_ptr] += (jump_amount >= 3) ? (-1) : 1;
            instr_ptr += jump_amount;

            step_i += 1;
        }

        std::cout << std::format("(part2) step_i: {}", step_i) << std::endl;
    }
};

int main() {
    std::filesystem::path file_path("./day5_input.txt");
    InputFile input_file(std::move(file_path));

    std::vector<int64_t> int_list = input_file.parse_day5();

    Solution solution;
    solution.part1(int_list);
    solution.part2(int_list);
    return 0;
}