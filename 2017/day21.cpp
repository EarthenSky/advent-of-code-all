#include <algorithm>
#include <array>
#include <fstream>
#include <format>
//#include <ranges>
#include <regex>
//#include <cctype>
//#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

using rulebook = std::unordered_map<std::string, std::string>;

class Day21FileParser : IFileParser<rulebook> {
public:
    using IFileParser<rulebook>::IFileParser;

    rulebook parse() const {
        rulebook rules;
        std::regex pattern(R"(([.#/]+) => ([.#/]+))");

        std::ifstream input_file(this->file_path);
        std::string line;
        while (std::getline(input_file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                std::string first_match = match[1].str();
                std::string second_match = match[2].str();
                if (first_match.size() == 5) {
                    rules[std::format(
                        "{}{}{}{}",
                        (char)(first_match[0] == '#'),  (char)(first_match[1] == '#'),
                        (char)(first_match[2+1] == '#'), (char)(first_match[3+1] == '#')
                    )] = std::format(
                        "{}{}{}{}{}{}{}{}{}",
                        (char)(second_match[0]   == '#'), (char)(second_match[1]   == '#'), (char)(second_match[2]   == '#'),
                        (char)(second_match[3+1] == '#'), (char)(second_match[4+1] == '#'), (char)(second_match[5+1] == '#'),
                        (char)(second_match[6+2] == '#'), (char)(second_match[7+2] == '#'), (char)(second_match[8+2] == '#')
                    );
                } else {
                    rules[std::format(
                        "{}{}{}{}{}{}{}{}{}",
                        (char)(first_match[0]   == '#'), (char)(first_match[1]   == '#'), (char)(first_match[2]   == '#'),
                        (char)(first_match[3+1] == '#'), (char)(first_match[4+1] == '#'), (char)(first_match[5+1] == '#'),
                        (char)(first_match[6+2] == '#'), (char)(first_match[7+2] == '#'), (char)(first_match[8+2] == '#')
                    )] = std::format(
                        "{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}",
                        (char)(second_match[0]    == '#'), (char)(second_match[1]    == '#'), (char)(second_match[2]    == '#'), (char)(second_match[3]    == '#'),
                        (char)(second_match[4+1]  == '#'), (char)(second_match[5+1]  == '#'), (char)(second_match[6+1]  == '#'), (char)(second_match[7+1]  == '#'),
                        (char)(second_match[8+2]  == '#'), (char)(second_match[9+2]  == '#'), (char)(second_match[10+2] == '#'), (char)(second_match[11+2] == '#'),
                        (char)(second_match[12+3] == '#'), (char)(second_match[13+3] == '#'), (char)(second_match[14+3] == '#'), (char)(second_match[15+3] == '#')
                    );
                }
            } else {
                std::runtime_error("ERROR: could not match line!");
            }
        }

        return rules;
    }
};

class Day21Solution {
    void rotated_cw(std::string& pattern) const {
        if (pattern.size() == 2 * 2) {
            pattern = std::format(
                "{}{}{}{}",
                pattern[2], pattern[0],
                pattern[3], pattern[1]
            );
        } else {
            pattern = std::format(
                "{}{}{}{}{}{}{}{}{}",
                pattern[6], pattern[3], pattern[0],
                pattern[7], pattern[4], pattern[1],
                pattern[8], pattern[5], pattern[2]
            );
        }
    }
    void flipped_over_y_axis(std::string& pattern) const {
        if (pattern.size() == 2 * 2) {
            pattern = std::format(
                "{}{}{}{}",
                pattern[1], pattern[0],
                pattern[3], pattern[2]
            );
        } else {
            pattern = std::format(
                "{}{}{}{}{}{}{}{}{}",
                pattern[2], pattern[1], pattern[0],
                pattern[5], pattern[4], pattern[3],
                pattern[8], pattern[7], pattern[6]
            );
        }
    }
    // TODO: should we accept a copy by value -> actually we should probably modify the value in this case
    /// @brief will try all 7 possible rotations and flips, then get the matching ruleset
    const std::string& get_matching_chunk(const rulebook& rules, std::string chunk_str) const {
        // std::cout << "got: " << chunk_str << std::endl; // ##./#.#/#..
        if (rules.contains(chunk_str))
            return rules.at(chunk_str);
        for (size_t rot_i = 1; rot_i < 4; rot_i++) {
            this->rotated_cw(chunk_str);
            // std::cout << "attempt: " << chunk_str << std::endl; // ##./#.#/#..
            if (rules.contains(chunk_str))
                return rules.at(chunk_str);
        }

        this->flipped_over_y_axis(chunk_str);
        //std::cout << "attempt(2): " << chunk_str << std::endl;
        if (rules.contains(chunk_str))
            return rules.at(chunk_str);
        for (size_t rot_i = 1; rot_i < 4; rot_i++) {
            this->rotated_cw(chunk_str);
            //std::cout << "attempt(3): " << chunk_str << std::endl;
            if (rules.contains(chunk_str))
                return rules.at(chunk_str);
        }

        throw std::runtime_error("ERROR: ran into a pattern not in the rulebook!");
    }
public:
    void part1(const rulebook& rules) const {
        size_t pattern_width = 3;
        std::vector<char> pattern = {
            false, true,  false,
            false, false, true,
            true,  true,  true
        };

        // perform rules to grow the pattern
        for (size_t iter_i = 0; iter_i < 18; iter_i++) {
            std::cout << "iter: " << iter_i << " w/ " << pattern_width << std::endl;
            
            if (pattern_width % 2 == 0) {
                size_t num_2x2_chunks = pattern.size() / (2*2);

                size_t new_pattern_width = (pattern_width / 2) * 3;
                std::vector<char> new_pattern(9 * num_2x2_chunks);
                for (size_t chunk_i = 0; chunk_i < num_2x2_chunks; chunk_i++) {
                    size_t chunk_x = chunk_i % (pattern_width / 2);
                    size_t chunk_y = chunk_i / (pattern_width / 2);
                    
                    std::string chunk_str = this->get_matching_chunk(rules, std::format(
                        "{}{}{}{}",
                        pattern[(chunk_y*2 + 0) * pattern_width + chunk_x*2 + 0],
                        pattern[(chunk_y*2 + 0) * pattern_width + chunk_x*2 + 1],
                        pattern[(chunk_y*2 + 1) * pattern_width + chunk_x*2 + 0],
                        pattern[(chunk_y*2 + 1) * pattern_width + chunk_x*2 + 1]
                    ));

                    // TODO: see if there are any (more) easy DRY gains!

                    new_pattern[(chunk_y*3 + 0) * new_pattern_width + chunk_x*3 + 0] = chunk_str[0];
                    new_pattern[(chunk_y*3 + 0) * new_pattern_width + chunk_x*3 + 1] = chunk_str[1];
                    new_pattern[(chunk_y*3 + 0) * new_pattern_width + chunk_x*3 + 2] = chunk_str[2];

                    new_pattern[(chunk_y*3 + 1) * new_pattern_width + chunk_x*3 + 0] = chunk_str[3];
                    new_pattern[(chunk_y*3 + 1) * new_pattern_width + chunk_x*3 + 1] = chunk_str[4];
                    new_pattern[(chunk_y*3 + 1) * new_pattern_width + chunk_x*3 + 2] = chunk_str[5];

                    new_pattern[(chunk_y*3 + 2) * new_pattern_width + chunk_x*3 + 0] = chunk_str[6];
                    new_pattern[(chunk_y*3 + 2) * new_pattern_width + chunk_x*3 + 1] = chunk_str[7];
                    new_pattern[(chunk_y*3 + 2) * new_pattern_width + chunk_x*3 + 2] = chunk_str[8];
                }
                pattern = std::move(new_pattern);
                pattern_width = new_pattern_width;
            } else {
                size_t num_3x3_chunks = pattern.size() / (3*3);

                size_t new_pattern_width = (pattern_width / 3) * 4;
                std::vector<char> new_pattern(16 * num_3x3_chunks);
                for (size_t chunk_i = 0; chunk_i < num_3x3_chunks; chunk_i++) {
                    size_t chunk_x = chunk_i % (pattern_width / 3);
                    size_t chunk_y = chunk_i / (pattern_width / 3);
                    
                    // TODO: replacing std::string with std::array or something more compact & easy to construct will
                    // probably be the fast way out
                    std::string chunk_str = this->get_matching_chunk(rules, std::format(
                        "{}{}{}{}{}{}{}{}{}",
                        pattern[(chunk_y*3 + 0) * pattern_width + chunk_x*3 + 0],
                        pattern[(chunk_y*3 + 0) * pattern_width + chunk_x*3 + 1],
                        pattern[(chunk_y*3 + 0) * pattern_width + chunk_x*3 + 2],
                        
                        pattern[(chunk_y*3 + 1) * pattern_width + chunk_x*3 + 0],
                        pattern[(chunk_y*3 + 1) * pattern_width + chunk_x*3 + 1],
                        pattern[(chunk_y*3 + 1) * pattern_width + chunk_x*3 + 2],

                        pattern[(chunk_y*3 + 2) * pattern_width + chunk_x*3 + 0],
                        pattern[(chunk_y*3 + 2) * pattern_width + chunk_x*3 + 1],
                        pattern[(chunk_y*3 + 2) * pattern_width + chunk_x*3 + 2]
                    ));

                    // TODO: see if there are any (more) easy DRY gains!

                    std::copy(
                        chunk_str.begin(), chunk_str.begin()+4,
                        new_pattern.begin() + (chunk_y*4 + 0) * new_pattern_width + chunk_x*4 + 0
                    );
                    std::copy(
                        chunk_str.begin()+4, chunk_str.begin()+8,
                        new_pattern.begin() + (chunk_y*4 + 1) * new_pattern_width + chunk_x*4 + 0
                    );
                    std::copy(
                        chunk_str.begin()+8, chunk_str.begin()+12,
                        new_pattern.begin() + (chunk_y*4 + 2) * new_pattern_width + chunk_x*4 + 0
                    );
                    std::copy(
                        chunk_str.begin()+12, chunk_str.begin()+16,
                        new_pattern.begin() + (chunk_y*4 + 3) * new_pattern_width + chunk_x*4 + 0
                    );
                }
                pattern = std::move(new_pattern);
                pattern_width = new_pattern_width;
            }
        
            // visualize the images!
            /*{
                size_t i = 0;
                for (char c : pattern) {
                    i += 1;
                    std::cout << c;
                    if (i % pattern_width == 0)
                        std::cout << std::endl;
                }
                std::cout << std::endl;
            }*/

            if (iter_i == 4) {
                size_t num_active_pixels = 0;

                // count num active pixels in the pattern
                for (char px_ch : pattern)
                    if (px_ch == (char)true)
                        num_active_pixels += 1;

                std::cout << "(part1) num_active_pixels: " << num_active_pixels << std::endl;
            }
        }

        size_t num_active_pixels = 0;

        // count num active pixels in the pattern
        for (char px_ch : pattern)
            if (px_ch == (char)true)
                num_active_pixels += 1;

        std::cout << "(part2) num_active_pixels: " << num_active_pixels << std::endl;
    }

    void part2(const rulebook& rules) const {
        // std::cout << "(part2) num_escaped_particles: " << num_escaped_particles << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day21_input.txt");
    Day21FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day21Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}