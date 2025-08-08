#include "helpers.h"

#include <algorithm> // std::count
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <ranges>
#include <string>

void part1(const Matrix<size_t, 2>& spreadsheet) {
    size_t another_checksum = 0;

    size_t height = spreadsheet.size()[0];
    size_t width = spreadsheet.size()[1];
    for (size_t yi = 0; yi < height; yi++) {
        size_t max_item = 0;
        size_t min_item = SIZE_MAX;
        for (size_t xi = 0; xi < width; xi++) {
            max_item = std::max(max_item, spreadsheet.get_const(yi, xi));
            min_item = std::min(min_item, spreadsheet.get_const(yi, xi));
        }

        another_checksum += (max_item - min_item);
    }

    std::cout << std::format("(part1) another_checksum: {}", another_checksum) << std::endl;
}

void part2(const Matrix<size_t, 2>& spreadsheet) {
    size_t another_checksum = 0;

    size_t height = spreadsheet.size()[0];
    size_t width = spreadsheet.size()[1];
    for (size_t yi = 0; yi < height; yi++) {
        auto find_divisor_pair = [&](){
            for (size_t ai = 0; ai < width; ai++) {
                size_t a = spreadsheet.get_const(yi, ai);
                for (size_t bi = 0; bi < width; bi++) {
                    if (ai == bi)
                        continue;

                    size_t b = spreadsheet.get_const(yi, bi);
                    if (a % b == 0)
                        // a cute early exit via lambdas!
                        return a / b;
                }
            }
            throw "row data didn't contain a pair of integers where one perfectly divides the other";
        };
        another_checksum += find_divisor_pair();
    }

    std::cout << std::format("(part2) another_checksum: {}", another_checksum) << std::endl;
}

void populate_spreadsheet(Matrix<size_t, 2>& spreadsheet, const std::basic_string<char>& file_contents) {
    size_t yi = 0;
    for (auto && line : std::views::split(file_contents, '\n')) {
        size_t xi = 0;
        for (auto && number_string : std::views::split(line, '\t')) {
            spreadsheet.get(yi, xi) = std::stoi(std::string(number_string.begin(), number_string.end()));
            xi += 1;
        }
        yi += 1;
    }
}

int main() {
    std::filesystem::path file_path = "day2_input.txt";
    std::ifstream input_file(file_path);

    // get size of the file
    input_file.seekg(0, std::ios::end);
    size_t file_size = input_file.tellg();
    if (file_size >= 4 * 1024 * 1024)
        throw "file too large";
    std::basic_string<char> file_contents(file_size, '\0');

    input_file.seekg(0, std::ios::beg);
    input_file.read(file_contents.data(), file_size);

    size_t height = std::count(file_contents.begin(), file_contents.end(), '\n') + 1;
    size_t width = (std::count(file_contents.begin(), file_contents.end(), '\t') / height) + 1;

    Matrix<size_t, 2> spreadsheet(height, width);
    populate_spreadsheet(spreadsheet, file_contents);

    std::cout << std::format("15,15 => {}", spreadsheet.get(15, 15)) << std::endl;
    std::cout << std::format("14,15 => {}", spreadsheet.get(14, 15)) << std::endl;

    part1(spreadsheet);
    part2(spreadsheet);

    return 0;
}