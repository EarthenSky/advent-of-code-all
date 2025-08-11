#include <algorithm>
#include <format>
#include <ranges>

#include "abstract_solution.h"
#include "file_input.h"

class Day8Solution : ISolution<Day8Data> {
    friend int main();

    void part1(const Day8Data& data) const {
        // evaluate registers
        std::unordered_map<std::string, int64_t> register_vals = data.register_vals;
        for (const Day8Instruction& inst : data.instruction_list) {
            if (
                (inst.cond == ">" && register_vals[inst.cond_reg] > inst.cond_val)
                || (inst.cond == ">=" && register_vals[inst.cond_reg] >= inst.cond_val)
                || (inst.cond == "<" && register_vals[inst.cond_reg] < inst.cond_val)
                || (inst.cond == "<=" && register_vals[inst.cond_reg] <= inst.cond_val)
                || (inst.cond == "==" && register_vals[inst.cond_reg] == inst.cond_val)
                || (inst.cond == "!=" && register_vals[inst.cond_reg] != inst.cond_val)
            ) {
                // std::cout << inst.offset << std::endl;
                register_vals[inst.target_reg] += inst.offset;
            }
        }

        // find max value in registers using fancy c++20 ranges!
        int64_t max_element = std::ranges::max_element(register_vals, {}, [](const auto& keyval) {
            return keyval.second;
        })->second;

        // TODO: use per-register mutexes in order to run this using 4 cores, instead of 1!

        std::cout << std::format("(part1) max_element: {}", max_element) << std::endl;
    }

    void part2(const Day8Data& data) const {
        // evaluate registers
        int64_t max_value_ever = 0;
        std::unordered_map<std::string, int64_t> register_vals = data.register_vals;
        // TODO: use per-register mutexes in order to run THIS using 4 cores, instead of 1!
        for (const Day8Instruction& inst : data.instruction_list) {
            if (
                (inst.cond == ">" && register_vals[inst.cond_reg] > inst.cond_val)
                || (inst.cond == ">=" && register_vals[inst.cond_reg] >= inst.cond_val)
                || (inst.cond == "<" && register_vals[inst.cond_reg] < inst.cond_val)
                || (inst.cond == "<=" && register_vals[inst.cond_reg] <= inst.cond_val)
                || (inst.cond == "==" && register_vals[inst.cond_reg] == inst.cond_val)
                || (inst.cond == "!=" && register_vals[inst.cond_reg] != inst.cond_val)
            ) {
                // std::cout << inst.offset << std::endl;
                register_vals[inst.target_reg] += inst.offset;
                max_value_ever = std::max(register_vals[inst.target_reg], max_value_ever);
            }
        }

        std::cout << std::format("(part2) max_value_ever: {}", max_value_ever) << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day8_input.txt");
    InputFile input_file(std::move(input_path));
    Day8Data data = input_file.parse_day8();

    Day8Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}