#include "file_input.h"

#include <ranges>
#include <fstream>

std::vector<int64_t> InputFile::parse_day5() const {
    std::vector<int64_t> int_list;
    std::ifstream input_file(this->file_path);

    std::string line;
    while (std::getline(input_file, line)) {
        int_list.push_back(std::stoi(line));
    }

    return int_list;
}

std::vector<uint8_t> InputFile::parse_day6() const {
    std::vector<uint8_t> int_list;
    std::ifstream input_file(this->file_path);

    std::string line;
    std::getline(input_file, line);
    for (auto&& number_str_range : std::views::split(line, '\t')) {
        std::string number_str(number_str_range.begin(), number_str_range.end());
        int_list.push_back((uint8_t) std::stoi(number_str));
    }

    return int_list;
}
