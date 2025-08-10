#include "file_input.h"

// #include <algorithm>
#include <ranges>
#include <regex>
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

void InputFile::parse_day7(
    std::unordered_map<std::string, std::vector<std::string>>& parent_child_mapping,
    std::unordered_map<std::string, std::string>& child_parent_mapping,
    std::unordered_map<std::string, size_t>& weight
) const {
    std::ifstream input_file(this->file_path);

    const std::string ARROW("->");

    std::string line;
    while (std::getline(input_file, line)) {
        if (line.find("->") != std::string::npos) {
            // TODO: read the docs on regex & matches
            std::regex pattern(R"((\S+) \((\d+)\) -> (.+))");
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                std::string parent = match[1];
                int parent_weight = std::stoi(match[2]);
                weight[parent] = parent_weight;
                
                std::vector<std::string> child_vector;

                const std::string COMMA_SPACE(", ");
                std::string comma_separated_children = match[3];
                for (auto&& item : std::views::split(comma_separated_children, COMMA_SPACE)) {
                    std::string child(item.begin(), item.end());
                    if (child_parent_mapping.contains(child))
                        throw std::runtime_error("unexpected child has more than 1 parent");

                    child_parent_mapping[child] = parent;
                    child_vector.push_back(child);
                }

                parent_child_mapping[parent] = std::move(child_vector);
            } else {
                throw std::runtime_error("failed regex match");
            }
        } else {
            std::regex pattern(R"((\S+) \((\d+)\))");
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                std::string parent = match[1];
                int parent_weight = std::stoi(match[2]);
                weight[parent] = parent_weight;
            } else {
                throw std::runtime_error("failed regex match");
            }
        }
    }
}

Day8Data InputFile::parse_day8() const {
    Day8Data data;

    std::ifstream input_file(this->file_path);
    std::string line;
    while (std::getline(input_file, line)) {
        std::regex pattern(R"(([a-z]+) (inc|dec) (\-?[0-9]+) if ([a-z]+) (>|<|==|!=|>=|<=) (\-?[0-9]+))");
        std::smatch match;
        if (std::regex_match(line, match, pattern)) {
            data.instruction_list.push_back(Day8Instruction{
                match[1],
                std::stoi(match[3]) * (match[2] == "inc" ? 1 : -1),
                match[4],
                match[5],
                std::stoi(match[6])
            });
            data.register_vals[match[1]] = 0;
            data.register_vals[match[4]] = 0;
        }
    }

    return data;
}
