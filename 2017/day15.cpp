#include <algorithm>
#include <fstream>
#include <format>
// #include <numeric>
// #include <ranges>
// #include <unordered_set>

// #include <cassert>

#include "abstract_solution.h"
#include "file_input.h"

class Day15FileParser : IFileParser<std::pair<size_t, size_t>> {
public:
    using IFileParser<std::pair<size_t, size_t>>::IFileParser;

    std::pair<size_t, size_t> parse() const {
        std::ifstream input_file(this->file_path);
        std::string line_a;
        std::getline(input_file, line_a);
        std::string line_b;
        std::getline(input_file, line_b);

        const size_t BEFORE_NUMBER = 24;
        return std::make_pair(
            std::stoi(
                std::string(line_a.begin() + BEFORE_NUMBER, line_a.end())
            ), std::stoi(
                std::string(line_b.begin() + BEFORE_NUMBER, line_b.end())
            )
        );
    }
};

class Day15Solution : ISolution<std::pair<size_t, size_t>> {
    const size_t FACTOR_A = 16807;
    const size_t FACTOR_B = 48271;
    const size_t FINAL_EQUIV_CLASS = 2147483647;
    const size_t N_PAIRS = 40 * 1000 * 1000;
    const size_t N_PAIRS_SMALLER = 5 * 1000 * 1000;

public:
    void part1(const std::pair<size_t, size_t>& starting_values) const {
        size_t total_equal_pairs = 0;
        size_t a = starting_values.first;
        size_t b = starting_values.second;
        for (size_t pair_i = 0; pair_i < N_PAIRS; pair_i++) {
            if ((a & 0xffff) == (b & 0xffff)) {
                total_equal_pairs += 1;
            }
            a = (a * FACTOR_A) % FINAL_EQUIV_CLASS;
            b = (b * FACTOR_B) % FINAL_EQUIV_CLASS;
        }

        std::cout << std::format("(part1) total_equal_pairs: {}", total_equal_pairs) << std::endl;
    }

    size_t get_next_a_value(size_t a) const {
        while (a % 4 != 0) {
            a = (a * FACTOR_A) % FINAL_EQUIV_CLASS;
        }
        return a;
    }
    size_t get_next_b_value(size_t b) const {
        while (b % 8 != 0) {
            b = (b * FACTOR_B) % FINAL_EQUIV_CLASS;
        }
        return b;
    }

    void part2(const std::pair<size_t, size_t>& starting_values) const {
        size_t total_equal_pairs = 0;
        size_t a = starting_values.first;
        size_t b = starting_values.second;
        for (size_t pair_i = 0; pair_i < N_PAIRS_SMALLER; pair_i++) {
            // each generator continues until it satisfies a criteria!
            a = get_next_a_value(a);
            b = get_next_b_value(b);
            if ((a & 0xffff) == (b & 0xffff)) {
                total_equal_pairs += 1;
            }

            a = (a * FACTOR_A) % FINAL_EQUIV_CLASS;
            b = (b * FACTOR_B) % FINAL_EQUIV_CLASS;
        }

        std::cout << std::format("(part2) total_equal_pairs: {}", total_equal_pairs) << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day15_input.txt");
    Day15FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day15Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}