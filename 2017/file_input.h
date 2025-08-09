#ifndef FILE_INPUT_H
#define FILE_INPUT_H

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class InputFile {
    std::filesystem::path file_path;

public:
    InputFile(std::filesystem::path&& file_path)
        : file_path(std::move(file_path)) {}

    std::vector<int64_t> parse_day5() const;
    std::vector<uint8_t> parse_day6() const;

    void parse_day7(
        std::unordered_map<std::string, std::vector<std::string>>&,
        std::unordered_map<std::string, std::string>&,
        std::unordered_map<std::string, size_t>&
    ) const;
};

#endif