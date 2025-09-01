#include <algorithm>
#include <array>
#include <fstream>
#include <format>
//#include <ranges>
#include <cstring>
#include <regex>
//#include <cctype>
//#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

struct StateRule {
    bool should_write_1;
    bool should_step_left;
    char next_state;
};
struct TuringMachineConfig {
    char starting_state;
    uint64_t checksum_steps;
    // even rules are applied when 0, odd when 1. rule / 2 is letter index -> starts at a, b, etc.. 
    std::vector<StateRule> rules;

    const StateRule& get_rule(char letter, bool is_one) const {
        // TODO: error checking (lowercase letter, etc...) would be nice here
        return rules[2 * (letter - 'A') + (size_t)is_one];
    }
};

class TuringMachine {
private:
    char current_state;
    std::vector<bool> tape_left;
    bool tape_center = false;
    std::vector<bool> tape_right;

public:
    int64_t cursor = 0;
    const TuringMachineConfig& config;

    TuringMachine(const TuringMachineConfig& config)
        : current_state(config.starting_state), config(config)
    {}

    void one_step() {
        bool cursor_value;
        if (cursor == 0) cursor_value = tape_center;
        else if (cursor < 0) cursor_value = tape_left[(-cursor) - 1];
        else if (cursor > 0) cursor_value = tape_right[cursor - 1];
        else throw std::runtime_error("unreachable");

        auto rule = config.get_rule(current_state, cursor_value);
        if (cursor == 0) tape_center = rule.should_write_1;
        else if (cursor < 0) tape_left[(-cursor) - 1] = rule.should_write_1;
        else if (cursor > 0) tape_right[cursor - 1] = rule.should_write_1;
        else throw std::runtime_error("unreachable");

        // only grow the tape when the cursor moves that far out!
        if (rule.should_step_left) {
            cursor -= 1;
            if (-cursor >= tape_left.size())
                tape_left.push_back(0);
        } else {
            cursor += 1;
            if (cursor >= tape_right.size())
                tape_right.push_back(0);
        }

        current_state = rule.next_state;
    }

    size_t count_ones() const noexcept {
        size_t num_ones = 0;
        num_ones += this->tape_center;
        for (bool v : this->tape_left)
            num_ones += v;
        for (bool v : this->tape_right)
            num_ones += v;
        return num_ones;
    }
};

class Day25FileParser : IFileParser<TuringMachineConfig> {
public:
    using IFileParser<TuringMachineConfig>::IFileParser;
    
    TuringMachineConfig parse() const {
        TuringMachineConfig tm;
        std::ifstream input_file(this->file_path);

        std::string start_line;
        std::getline(input_file, start_line);
        std::smatch start_match;
        if (std::regex_match(start_line, start_match, std::regex(R"(Begin in state ([A-Z]).)"))) {
            tm.starting_state = start_match[1].str()[0];
        } else throw std::runtime_error("failed to match start line");
        
        std::string checksum_line;
        std::getline(input_file, checksum_line);
        std::smatch checksum_match;
        if (std::regex_match(checksum_line, checksum_match, std::regex(R"(Perform a diagnostic checksum after ([0-9]+) steps.)"))) {
            tm.checksum_steps = std::stoull(checksum_match[1].str());
        } else throw std::runtime_error("failed to match checksum line");

        // NOTE: state A is expected to come first, followed by state B, etc...
        std::string empty_line;
        while (std::getline(input_file, empty_line)) {
            if (empty_line.size() != 0)
                throw std::runtime_error("did not expect line to have contents; something is wrong!");

            // get the 9 lines that make up the current state
            std::array<std::string,9> lines;
            for (size_t i = 0; i < lines.size(); i++)
                std::getline(input_file, lines[i]);

            // NOTE: We don't have to be super anal parsing here; the empty line check should suffice finding bugs
            StateRule zero_case;
            zero_case.should_write_1   = lines[2].ends_with("Write the value 1.");
            zero_case.should_step_left = lines[3].ends_with("Move one slot to the left.");
            zero_case.next_state       = lines[4][lines[4].size()-2]; // NOTE: delimiter (\n) is not stored by std::getline

            StateRule one_case;
            one_case.should_write_1   = lines[6].ends_with("Write the value 1.");
            one_case.should_step_left = lines[7].ends_with("Move one slot to the left.");
            one_case.next_state       = lines[8][lines[8].size()-2];

            std::cout << "(0) ::" << std::endl;
            std::cout << zero_case.should_write_1 << std::endl;
            std::cout << zero_case.should_step_left << std::endl;
            std::cout << zero_case.next_state << std::endl;
            std::cout << "(1) ::" << std::endl;
            std::cout << one_case.should_write_1 << std::endl;
            std::cout << one_case.should_step_left << std::endl;
            std::cout << one_case.next_state << std::endl;

            tm.rules.push_back(zero_case);
            tm.rules.push_back(one_case);
        }

        std::cout << "rules size: " << tm.rules.size() << std::endl;
        return tm;
    }
};

class Day25Solution {
public:
    void part1(const TuringMachineConfig& config) const {
        TuringMachine tm(config);
        for (size_t i = 0; i < tm.config.checksum_steps; i++) {
            if (i % (500 * 1000) == 0)
                std::cout << i << "\n";
            tm.one_step();
        }
        //if (tm.cursor != 0) 
        //    throw std::runtime_error(std::format("invalid turing machine final state! (cursor={})", tm.cursor));

        std::cout << "(part1) num_ones: " << tm.count_ones() << std::endl;
    }

    void part2(const TuringMachineConfig& config) const {
        // std::cout << "(part2) longest_strongest: " << strongest << std::endl;
    }
};

int main() {
    Day25FileParser parser(std::filesystem::path("./day25_input.txt"));
    auto data = parser.parse();

    Day25Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}