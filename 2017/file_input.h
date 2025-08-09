#ifndef FILE_INPUT_H
#define FILE_INPUT_H

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class InputFile {
    std::filesystem::path file_path;

public:
    InputFile(std::filesystem::path&& file_path)
        : file_path(std::move(file_path)) {}

    std::vector<int64_t> parse_day5() const;
};

#endif