#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

#include <string>

void part1(const std::basic_string<char>& line) {
    size_t sum_of_equal_digits = 0;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        // circular list
        // char c_next = (i == line.size()-1) ? line[0] : line[i+1];
        char c_next = line[(i+1) % line.size()];
        
        // sanity check
        if (!isdigit(c))
            throw std::format("char at index {} was not digit", i);

        if (c == c_next) {
            sum_of_equal_digits += static_cast<size_t>(c - '0');
        }
    }

    std::cout << std::format("(part1) sum_of_equal_digits: {}\n", sum_of_equal_digits);
}

void part2(const std::basic_string<char>& line) {
    size_t captcha_sum = 0;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        char c_next = line[(i + line.size()/2) % line.size()];

        if (c == c_next) {
            captcha_sum += static_cast<size_t>(c - '0');
        }
    }

    std::cout << std::format("(part2) captcha_sum: {}\n", captcha_sum);
}

int main() {
    std::filesystem::path file_path = "day1_input.txt";
    std::ifstream input_file(file_path);

    std::basic_string<char> line;
    std::getline(input_file, line);

    part1(line);
    part2(line);

    return 0;
}