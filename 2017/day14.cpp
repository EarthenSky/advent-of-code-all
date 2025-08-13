#include <algorithm>
#include <fstream>
#include <format>
#include <numeric>
#include <ranges>

#include <cassert>

#include "day10_solution.h"

#include "abstract_solution.h"
#include "file_input.h"

class Day14FileParser : IFileParser<std::string> {
public:
    using IFileParser<std::string>::IFileParser;

    std::string parse() const {
        std::ifstream input_file(this->file_path);
        std::string line;
        std::getline(input_file, line);
        return line;
    }
};

class Day14Solution : ISolution<std::string> {
public:
    void part1(const std::string& input_str) const {
        const size_t DRIVE_WIDTH = 128;
        std::array<bool, DRIVE_WIDTH*DRIVE_WIDTH> grid;

        Day10Solution solution;
        for (size_t i = 0; i < DRIVE_WIDTH; i++) {
            auto dense_hash = solution.compute_hash(input_str + "-" + std::to_string(i));
            size_t x = 0;
            for (uint8_t u8 : dense_hash) {
                for (size_t xi = x; xi < x+8; xi++)
                    grid[i * DRIVE_WIDTH + xi] = (u8 & (0x01 << (xi % 8))) != 0;
                x += 8;
            }
        }

        size_t num_ones = 0;
        for (bool bit : grid) {
            if (bit)
                num_ones += 1;
        }
        std::cout << std::format("(part1) num_ones: {}", num_ones) << std::endl;        
    }

    void part2(const std::string& layers) const {
   
    }
};

int main() {
    std::filesystem::path input_path("./day14_input.txt");
    Day14FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day14Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}