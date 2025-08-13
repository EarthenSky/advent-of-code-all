#include <format>
#include <fstream>
#include <ranges>

#include "day10_solution.h"
#include "file_input.h"

class Day10FileParser : IFileParser<std::vector<size_t>> {
public:
    using IFileParser<std::vector<size_t>>::IFileParser;

    std::vector<size_t> parse() const {
        std::ifstream input_file(this->file_path);
        
        std::string line;
        std::getline(input_file, line);

        std::vector<size_t> numbers;
        for (auto&& item : std::views::split(line, ',')) {
            numbers.push_back(
                (size_t) std::stoi(std::string(item.begin(), item.end()))
            );
        }

        return numbers;
    }
};

int main() {
    std::filesystem::path input_path("./day10_input.txt");
    Day10FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day10Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}