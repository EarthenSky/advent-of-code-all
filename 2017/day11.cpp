#include <algorithm>
#include <format>
#include <fstream>
#include <ranges>

#include "abstract_solution.h"
#include "file_input.h"

enum Direction {
    NORTH = 0,
    NORTH_EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    NORTH_WEST
};

class Day11FileParser : IFileParser<std::vector<Direction>> {
public:
    using IFileParser<std::vector<Direction>>::IFileParser;

    std::vector<Direction> parse() const {
        std::ifstream input_file(this->file_path);
        
        std::string line;
        std::getline(input_file, line);

        std::vector<Direction> directions;
        for (auto&& item : std::views::split(line, ',')) {
            std::string direction_str(item.begin(), item.end());
            if (direction_str == "n") {
                directions.push_back(NORTH);
            } else if (direction_str == "ne") {
                directions.push_back(NORTH_EAST);
            } else if (direction_str == "se") {
                directions.push_back(SOUTH_EAST);
            } else if (direction_str == "s") {
                directions.push_back(SOUTH);
            } else if (direction_str == "sw") {
                directions.push_back(SOUTH_WEST);
            } else if (direction_str == "nw") {
                directions.push_back(NORTH_WEST);
            }
        }
        return directions;
    }
};

class Day11Solution : ISolution<std::vector<Direction>> {
public:
    void part1(const std::vector<Direction>& steps) const {
        // y     x
        //   y x
        //   x y
        // z z z z
        int64_t x = 0;
        int64_t y = 0;
        int64_t z = 0;

        for (Direction step : steps) {
            if (step == NORTH) {
                y += 1;
            } else if (step == NORTH_EAST) {
                x += 1;
            } else if (step == SOUTH_EAST) {
                z -= 1;
            } else if (step == SOUTH) {
                y -= 1;
            } else if (step == SOUTH_WEST) {
                x -= 1;
            } else if (step == NORTH_WEST) {
                z += 1;
            }
        }

        std::cout << std::format("(part1) total distance: {}",
            // 3 different ways to use 2 axis to get to a target location,
            // converting the last axis into some combination of the other two.
            //
            // All ways are eual!
            // 
            //abs(x - z) + abs(y + z),
            //abs(y + x) + abs(z - x),
            abs(x + y) + abs(z + y)
        ) << std::endl;
    }
    void part2(const std::vector<Direction>& steps) const {
        int64_t x = 0;
        int64_t y = 0;
        int64_t z = 0;

        size_t max_distance = 0;

        for (Direction step : steps) {
            if (step == NORTH) {
                y += 1;
            } else if (step == NORTH_EAST) {
                x += 1;
            } else if (step == SOUTH_EAST) {
                z -= 1;
            } else if (step == SOUTH) {
                y -= 1;
            } else if (step == SOUTH_WEST) {
                x -= 1;
            } else if (step == NORTH_WEST) {
                z += 1;
            }

            max_distance = std::max(
                (size_t) abs(x + y) + (size_t) abs(z + y),
                max_distance
            );
        }

        std::cout << std::format("(part2) total distance: {}", max_distance) << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day11_input.txt");
    Day11FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day11Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}