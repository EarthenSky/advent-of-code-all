#include <algorithm>
#include <array>
#include <fstream>
#include <format>
#include <unordered_set>
#include <unordered_map>
//#include <ranges>
#include <cstring>
#include <regex>
#include <cassert>
//#include <cctype>
//#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

struct Operand {
    bool is_register;
    int64_t value; // if is_register, value must be in [0,7]
};
struct Instruction {
    std::string operation;
    Operand left;
    Operand right;
};

Operand create_op(std::string s) {
    if (s[0] == '-' || std::isdigit(s[0])) {
        // std::cout << s << " == false" << std::endl;
        return Operand{false, std::stoll(s)};
    } else {
        // std::cout << s << " == true" << std::endl;
        return Operand{true, s[0] - 'a'};
    }
}
int64_t get_op_value(const std::array<int64_t, 8>& registers, Operand op) {
    if (op.is_register) {
        return registers[op.value];
    } else {
        return op.value;
    }
}

class Day23FileParser : IFileParser<std::vector<Instruction>> {
public:
    using IFileParser<std::vector<Instruction>>::IFileParser;
    
    std::vector<Instruction> parse() const {
        std::vector<Instruction> instructions;

        std::regex pattern(R"((set|sub|mul|jnz) ([a-z]|[-0-9]+) ([a-z]|[-0-9]+))");

        std::ifstream input_file(this->file_path);
        std::string line;
        int y = 0;
        while (std::getline(input_file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                instructions.push_back(Instruction{
                    match[1].str(),
                    create_op(match[2].str()),
                    create_op(match[3].str())
                });
            } else {
                throw std::runtime_error("unable to match line!");
            }
        }

        return instructions;
    }
};

class Day23Solution {
public:
    void part1(const std::vector<Instruction>& rules) const {
        std::array<int64_t, 8> registers;
        std::memset(registers.data(), 0, registers.size());

        size_t num_multiplies = 0;
        int64_t iptr = 0;
        while (iptr >= 0 && iptr < rules.size()) {
            // std::cout << "OP: " << rules[iptr].operation << std::endl;
            if (rules[iptr].operation == "set") {
                assert(rules[iptr].left.is_register);
                registers[rules[iptr].left.value] = get_op_value(registers, rules[iptr].right);
                iptr += 1;
            } else if (rules[iptr].operation == "sub") {
                assert(rules[iptr].left.is_register);
                registers[rules[iptr].left.value] -= get_op_value(registers, rules[iptr].right);
                iptr += 1;
            } else if (rules[iptr].operation == "mul") {
                assert(rules[iptr].left.is_register);
                num_multiplies += 1;
                registers[rules[iptr].left.value] *= get_op_value(registers, rules[iptr].right);
                iptr += 1;
            } else if (rules[iptr].operation == "jnz") {
                if (get_op_value(registers, rules[iptr].left) != 0) {
                    iptr += get_op_value(registers, rules[iptr].right);
                } else {
                    iptr += 1;
                }
            } else {
                throw std::runtime_error("ERROR: encountered invalid operation");
            }
        }

        std::cout << "(part1) num_multiplies: " << num_multiplies << std::endl;
    }

    void part2(const std::vector<Instruction>& rules) const {

        // std::cout << "(part2) num_infection_bursts: " << num_infection_bursts << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day23_input.txt");
    Day23FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day23Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}