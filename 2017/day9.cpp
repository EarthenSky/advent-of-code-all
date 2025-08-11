#include <format>
#include <fstream>

#include "abstract_solution.h"
#include "file_input.h"

struct Day9Data {
    std::string line;
};

class Day9FileParser : IFileParser<std::string> {
public:
    using IFileParser<std::string>::IFileParser;

    std::string parse() const {
        std::ifstream input_file(this->file_path);
        
        std::string line;
        std::getline(input_file, line);

        return line;
    }
};

class Day9Solution : ISolution<std::string> {
    size_t count_score(const std::string& line, size_t& i, size_t depth) const {
        if (i+1 >= line.size())
            throw std::runtime_error("did not expect parsing to START on the last char (or later!)");
        else if (line[i] == '{' and line[i+1] == '}') {
            i += 2;
            return depth;
        } else if (line[i] == '{') {
            size_t group_score = 0;
            i += 1;

            // recursively parse a list of stuff of size >= 1
            while (true) {
                group_score += count_score(line, i, depth + 1);
                // skip all "ignored" characters
                while (line[i] == '!') {
                    i += 2;
                }

                if (line[i] == '}') {
                    i += 1;
                    return group_score + depth;
                } else if (line[i] == ',') {
                    i += 1;
                } else {
                    throw std::runtime_error("unexpected char after group!");
                }
            }
        } else if (line[i] == '<') {
            while (true) {
                char current_char = line[i];
                if (current_char == '>') {
                    i += 1;
                    // garbage is worth nothing
                    return 0;
                } else if (current_char == '!') {
                    i += 2;
                } else {
                    i += 1;
                }
            }
        } else {
            throw std::runtime_error("unexpected start of group character!");
        }
    }


    size_t count_garbage(const std::string& line, size_t& i) const {
        if (i+1 >= line.size())
            throw std::runtime_error("did not expect parsing to START on the last char (or later!)");
        else if (line[i] == '{' and line[i+1] == '}') {
            i += 2;
            return 0;
        } else if (line[i] == '{') {
            i += 1;

            // recursively parse a list of stuff of size >= 1
            size_t sub_garbage = 0;
            while (true) {
                sub_garbage += count_garbage(line, i);

                // skip all "ignored" characters
                while (line[i] == '!') {
                    i += 2;
                }

                if (line[i] == '}') {
                    i += 1;
                    return sub_garbage;
                } else if (line[i] == ',') {
                    i += 1;
                } else {
                    throw std::runtime_error("unexpected char after group!");
                }
            }
        } else if (line[i] == '<') {
            i += 1;

            size_t garbage_size = 0;
            while (true) {
                char current_char = line[i];
                if (current_char == '>') {
                    i += 1;
                    return garbage_size;
                } else if (current_char == '!') {
                    i += 2;
                } else {
                    i += 1;
                    garbage_size += 1;
                }
            }
        } else {
            throw std::runtime_error("unexpected start of group character!");
        }
    }

public:
    void part1(const std::string& line) const {
        size_t i = 0;
        size_t total_score = count_score(line, i, 1);
        std::cout << std::format("(part1) total_score: {}", total_score) << std::endl;
    }

    void part2(const std::string& line) const {
        size_t i = 0;
        size_t garbage_count = count_garbage(line, i);
        std::cout << std::format("(part2) garbage_count: {}", garbage_count) << std::endl;
    }
};

int main() {
    std::filesystem::path puzzle_input_path("./day9_input.txt");
    Day9FileParser parser(std::move(puzzle_input_path));
    auto data = parser.parse();

    Day9Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}