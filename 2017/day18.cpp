#include <algorithm>
#include <fstream>
#include <format>
#include <regex>
#include <variant>
#include <cctype>

#include "abstract_solution.h"
#include "file_input.h"

struct Instruction {
    std::string name;
    std::variant<int, char> first;
    // second will be monostate for single param instructions
    std::variant<int, char, std::monostate> second;
};

using instruction_list = std::vector<Instruction>;

class Day18FileParser : IFileParser<instruction_list> {
private:
    std::variant<int,char> parse_reg_or_int(std::string match_str) const noexcept {
        std::variant<int,char> result;
        char ch0 = match_str[0];
        if (std::isdigit(ch0) || ch0 == '-')
            result.emplace<int>(std::stoi(match_str));
        else
            result.emplace<char>(ch0);
        return result;
    }
    // the compiler should be able to auto generate this func ;-;
    std::variant<int,char,std::monostate> to_optional_variant(std::variant<int,char> operand) const noexcept {
        std::variant<int,char,std::monostate> result;
        if (std::holds_alternative<int>(operand)) {
            result.emplace<int>(std::get<int>(operand));
        } else {
            result.emplace<char>(std::get<char>(operand));
        }
        return result;
    }
public:
    using IFileParser<instruction_list>::IFileParser;

    instruction_list parse() const {
        instruction_list instructions;

        std::regex pattern(R"((snd|set|add|mul|mod|rcv|jgz) ([a-z]|[-0-9]*) ?([a-z]|[-0-9]*))");

        std::ifstream input_file(this->file_path);
        std::string line;
        while (std::getline(input_file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                std::variant<int, char, std::monostate> last_instruction;
                if (match[3].length() == 0)
                    last_instruction = std::monostate{};
                else
                    last_instruction = to_optional_variant(parse_reg_or_int(match[3].str()));

                instructions.push_back(
                    Instruction {
                        match[1].str(),
                        parse_reg_or_int(match[2].str()),
                        last_instruction
                    }
                );
            } else {
                std::cout << line << std::endl;
                throw std::runtime_error("invalid line; could not match pattern");
            }
        }
        return instructions;
    }
};

// TODO: understand this
template <typename Variant>
std::string variant_to_string(const Variant& v) {
    return std::visit([](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>)
            return std::format("{}", val);
        else if constexpr (std::is_same_v<T, char>)
            return std::format("{}", val);
        else if constexpr (std::is_same_v<T, std::monostate>)
            return "<std::monostate>";
    }, v);
}

class Day18Solution {
private:
    /// @return whether the evaluate instruction should terminate or not. True means terminate program!
    bool evaluate_instruction(
        const instruction_list& instructions,
        std::vector<int64_t>& registers,
        int64_t& iptr,
        int64_t& most_recently_played_sound,
        int64_t& recovered_frequency
    ) const {
        if (instructions[iptr].name == "snd") {
            int value;
            if (std::holds_alternative<int>(instructions[iptr].first)) {
                value = std::get<int>(instructions[iptr].first);
            } else {
                value = registers[
                    std::get<char>(instructions[iptr].first) - 'a'
                ];
            }
            most_recently_played_sound = value;
            iptr += 1;
        } else if (instructions[iptr].name == "set") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to SET must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            if (std::holds_alternative<int>(instructions[iptr].second)) {
                target_reg = std::get<int>(instructions[iptr].second);
            } else {
                target_reg = registers[
                    std::get<char>(instructions[iptr].second) - 'a'
                ];
            }
            iptr += 1;
        } else if (instructions[iptr].name == "add") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to ADD must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            if (std::holds_alternative<int>(instructions[iptr].second)) {
                target_reg += std::get<int>(instructions[iptr].second);
            } else {
                target_reg += registers[
                    std::get<char>(instructions[iptr].second) - 'a'
                ];
            }
            iptr += 1;
        } else if (instructions[iptr].name == "mul") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to MUL must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            if (std::holds_alternative<int>(instructions[iptr].second)) {
                target_reg *= std::get<int>(instructions[iptr].second);
            } else {
                target_reg *= registers[
                    std::get<char>(instructions[iptr].second) - 'a'
                ];
            }
            iptr += 1;
        } else if (instructions[iptr].name == "mod") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to MOD must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            if (std::holds_alternative<int>(instructions[iptr].second)) {
                target_reg %= std::get<int>(instructions[iptr].second);
            } else {
                target_reg %= registers[
                    std::get<char>(instructions[iptr].second) - 'a'
                ];
            }
            iptr += 1;
        } else if (instructions[iptr].name == "rcv") {
            int64_t reg_value;
            if (std::holds_alternative<int>(instructions[iptr].first)) {
                reg_value = std::get<int>(instructions[iptr].first);
            } else {
                reg_value = registers[
                    std::get<char>(instructions[iptr].first) - 'a'
                ];
            }

            if (reg_value != 0) {
                recovered_frequency = most_recently_played_sound;
                return true;
            } else {
                iptr += 1;
            }
        } else if (instructions[iptr].name == "jgz") {
            int64_t first_value;
            if (std::holds_alternative<int>(instructions[iptr].first))
                first_value = std::get<int>(instructions[iptr].first);
            else
                first_value = registers[
                    std::get<char>(instructions[iptr].first) - 'a'
                ];

            int64_t second_value;
            if (std::holds_alternative<int>(instructions[iptr].second))
                second_value = std::get<int>(instructions[iptr].second);
            else
                second_value = registers[
                    std::get<char>(instructions[iptr].second) - 'a'
                ];
            
            if (first_value > 0) {
                iptr += second_value;
            } else {
                iptr += 1;
            }
        } else {
            throw std::runtime_error("got invalid instruction name!");
        }

        return false;
    }

    /// @return whether this program is waiting for its recv queue
    bool evaluate_instruction_v2(
        const instruction_list& instructions,
        std::vector<int64_t>& registers,

        std::vector<int64_t>& send_queue,
        std::vector<int64_t>& recv_queue,
        int64_t& iptr
    ) const {
        auto get_int_from_instruction = [&](auto the_variant) {
            if (std::holds_alternative<int>(the_variant)) {
                return (int64_t) std::get<int>(the_variant);
            } else {
                return registers[std::get<char>(the_variant) - 'a'];
            }
        };

        if (instructions[iptr].name == "snd") {
            int value;
            if (std::holds_alternative<int>(instructions[iptr].first)) {
                value = std::get<int>(instructions[iptr].first);
            } else {
                value = registers[
                    std::get<char>(instructions[iptr].first) - 'a'
                ];
            }
            send_queue.push_back(value);
            iptr += 1;
        } else if (instructions[iptr].name == "set") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to SET must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            target_reg = get_int_from_instruction(instructions[iptr].second);
            iptr += 1;
        } else if (instructions[iptr].name == "add") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to ADD must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            target_reg += get_int_from_instruction(instructions[iptr].second);
            iptr += 1;
        } else if (instructions[iptr].name == "mul") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to MUL must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            target_reg *= get_int_from_instruction(instructions[iptr].second);
            iptr += 1;
        } else if (instructions[iptr].name == "mod") {
            if (std::holds_alternative<int>(instructions[iptr].first))
                throw std::runtime_error("first arg to MOD must be a register");
    
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            target_reg %= get_int_from_instruction(instructions[iptr].second);
            iptr += 1;
        } else if (instructions[iptr].name == "rcv") {
            int64_t& target_reg = registers[
                std::get<char>(instructions[iptr].first) - 'a'
            ];
            if (recv_queue.empty()) {
                return true;
            } {
                target_reg = recv_queue[0];
                // NOTE: this is indeed an O(n) operation!
                recv_queue.erase(recv_queue.begin());
                iptr += 1;
            }
        } else if (instructions[iptr].name == "jgz") {
            int64_t first_value = get_int_from_instruction(instructions[iptr].first);
            int64_t second_value = get_int_from_instruction(instructions[iptr].second);
            if (first_value > 0) {
                iptr += second_value;
            } else {
                iptr += 1;
            }
        } else {
            throw std::runtime_error("got invalid instruction name!");
        }
        return false;
    }
public:
    void part1(const instruction_list& instructions) const {
        std::vector<int64_t> registers(27);
        for (size_t i = 0; i < registers.size(); i++)
            registers[i] = 0;

        int64_t most_recently_played_sound = -1;
        int64_t recovered_frequency = -1;
        int64_t iptr = 0;
        while (iptr >= 0 && iptr < instructions.size()) {
            //std::cout
            //    << instructions[iptr].name << " "
            //    << variant_to_string(instructions[iptr].first) << " "
            //    << variant_to_string(instructions[iptr].second) << std::endl;

            bool should_terminate = evaluate_instruction(
                instructions,
                registers,
                iptr,
                most_recently_played_sound,
                recovered_frequency
            );

            if (should_terminate)
                break;
        }

        std::cout << "(part1) recovered_frequency: " << recovered_frequency << std::endl;
    }

    void part2(const instruction_list& instructions) {
        std::vector<int64_t> p0_registers(27);
        for (size_t i = 0; i < p0_registers.size(); i++)
            p0_registers[i] = 0;

        std::vector<int64_t> p1_registers(27);
        for (size_t i = 0; i < p1_registers.size(); i++)
            p1_registers[i] = 0;

        p0_registers['p'-'a'] = 0;
        p1_registers['p'-'a'] = 1;

        std::vector<int64_t> p0_input_queue;
        std::vector<int64_t> p1_input_queue;

        bool p0_is_waiting = false;
        bool p1_is_waiting = false;

        int64_t p0_iptr = 0;
        int64_t p1_iptr = 0;
        
        size_t num_p1_sends = 0;

        while (!(p0_is_waiting && p1_is_waiting)) {
            p0_is_waiting = evaluate_instruction_v2(
                instructions,
                p0_registers,
                p0_input_queue,
                p1_input_queue,
                p0_iptr
            );
            
            size_t p0_input_size_before = p1_input_queue.size();
            p1_is_waiting = evaluate_instruction_v2(
                instructions,
                p1_registers,
                p1_input_queue,
                p0_input_queue,
                p1_iptr
            );
            if (p1_input_queue.size() > p0_input_size_before)
                num_p1_sends += 1;
        }

        std::cout << "(part2) num_p1_sends: " << num_p1_sends << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day18_input.txt");
    Day18FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day18Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}