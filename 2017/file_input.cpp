#include "file_input.h"

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
