#include <algorithm>
#include <fstream>
#include <format>
#include <regex>
#include <variant>
#include <cctype>

#include "abstract_solution.h"
#include "file_input.h"

using string_matrix = std::vector<std::string>;

class Day19FileParser : IFileParser<string_matrix> {
public:
    using IFileParser<string_matrix>::IFileParser;

    string_matrix parse() const {
        string_matrix matrix;

        std::ifstream input_file(this->file_path);
        std::string line;
        while (std::getline(input_file, line))
            matrix.push_back(line);

        return matrix;
    }
};

enum Direction {
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
};

class Day19Solution {
public:
    void part1(const string_matrix& matrix) const {
        size_t starting_x = std::find(matrix[0].begin(), matrix[0].end(), '|') - matrix[0].begin();
        std::cout << "START 2" << std::endl;

        std::vector<char> letters_observed;

        Direction direction = SOUTH;
        size_t x = starting_x;
        size_t y = 0;
        while (true) {
            std::vector<Direction> valid_directions;

            if (std::isalpha(matrix[y][x])) {
                //std::cout << "\t" << matrix[y][x] << std::endl;
                letters_observed.push_back(matrix[y][x]);

                // NOTE: the path must end on a letter or a plus
                size_t num_possible_directions = (
                    (matrix[y][x-1] != ' ')
                    + (matrix[y+1][x] != ' ')
                    + (matrix[y][x+1] != ' ')
                    + (matrix[y-1][x] != ' ')
                );
                if (num_possible_directions == 1)
                    break;
            }

            if (matrix[y][x] == '+') {
                size_t num_possible_directions = (
                    (matrix[y][x-1] != ' ')
                    + (matrix[y+1][x] != ' ')
                    + (matrix[y][x+1] != ' ')
                    + (matrix[y-1][x] != ' ')
                );

                //std::cout << "dirs: " << num_possible_directions << std::endl;
                if (num_possible_directions == 1) {
                    break;
                } else if (num_possible_directions != 2) {
                    throw std::runtime_error("expected only 1 possible path to go!");
                } 

                if (matrix[y][x-1] != ' ' && direction != EAST) {
                    direction = WEST;
                } else if (matrix[y+1][x] != ' ' && direction != NORTH) {
                    direction = SOUTH;
                } else if (matrix[y][x+1] != ' ' && direction != WEST) {
                    direction = EAST;
                } else if (matrix[y-1][x] != ' ' && direction != SOUTH) {
                    direction = NORTH;
                }
            }

            if (direction == NORTH) {
                y -= 1;
            } else if (direction == EAST) {
                x += 1;
            } else if (direction == SOUTH) {
                y += 1;
            } else if (direction == WEST) {
                x -= 1;
            }
        }

        std::cout << "(part1) : ";
        for (auto ch : letters_observed)
            std::cout << ch;
        std::cout << std::endl;

    }

    void part2(const string_matrix& matrix) {
        std::cout << "(part2) : " << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day19_input.txt");
    Day19FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day19Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}