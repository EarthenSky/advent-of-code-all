#include <algorithm>
#include <array>
#include <fstream>
#include <format>
#include <unordered_set>
#include <unordered_map>
//#include <ranges>
#include <list>
#include <cstring>
#include <regex>
#include <cassert>
//#include <cctype>
//#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

using list_pairs = std::vector<std::pair<int, int>>;

class Day24FileParser : IFileParser<list_pairs> {
public:
    using IFileParser<list_pairs>::IFileParser;
    
    list_pairs parse() const {
        list_pairs pairs;
        std::regex pattern(R"(([0-9]+)/([0-9]+))");

        std::ifstream input_file(this->file_path);
        std::string line;
        while (std::getline(input_file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                pairs.push_back(std::make_pair(
                    std::stoi(match[1].str()),
                    std::stoi(match[2].str())
                ));
            } else {
                throw std::runtime_error("unable to match line!");
            }
        }

        return pairs;
    }
};

class Day24Solution {
private:
    // TODO: we can cache this by computing an easy hash of remaining components (assume each element is in [0,64])
    int bridge_strength(
        // using list instead of vector for inserts & stuff (nearly) doubled performance!
        std::list<std::pair<int,int>>& remaining_components,
        size_t num_remaining,
        int next_num_pins
    ) const {
        // base case to avoid the unneeded copy!
        if (num_remaining == 0)
            return 0;

        int max_strength = 0;

        // filter on input num_pins (TODO: binary search would probably be fastest? -> possibly)
        auto component_it = remaining_components.begin();
        for (size_t i = 0; i < num_remaining; i++) {
            auto curr_component = *component_it;

            if (curr_component.first == next_num_pins) {
                // next will alwyas exist b/c component_it only becomes it.end() at the END of the final loop 
                auto component_next_it = std::next(component_it);
                remaining_components.splice(remaining_components.end(), remaining_components, component_it);

                // Recursively compute strength using a smaller set of remaining components -> only uses O(n^2)
                // memory, where n ~= 50, so we're doing good!
                int strength = curr_component.first + curr_component.second;
                int recursive_strength = bridge_strength(remaining_components, num_remaining - 1, curr_component.second);
                max_strength = std::max(
                    max_strength,
                    recursive_strength + strength
                );

                // NOTE: b/c of this insert, the order should be fine!
                remaining_components.splice(component_next_it, remaining_components, component_it);
            }

            // NOTE: ports are allowed to be backwards! try all the ports again, but backwards!
            if (
                curr_component.second == next_num_pins
                // NOTE: adding this case tripled performance!
                && curr_component.first != curr_component.second
            ) {
                auto component_next_it = std::next(component_it);
                remaining_components.splice(remaining_components.end(), remaining_components, component_it);

                // Recursively compute strength using a smaller set of remaining components -> only uses O(n^2)
                // memory, where n ~= 50, so we're doing good!
                int strength = curr_component.first + curr_component.second;
                int recursive_strength = bridge_strength(remaining_components, num_remaining - 1, curr_component.first);
                max_strength = std::max(
                    max_strength,
                    recursive_strength + strength
                );

                remaining_components.splice(component_next_it, remaining_components, component_it);
            }

            component_it++;
        }

        return max_strength;
    }

    void longest_strongest_bridge(
        std::list<std::pair<int,int>>& remaining_components,
        size_t num_remaining,
        int next_num_pins,
        int& longest,
        int& strongest
    ) const {
        // base case for clarity
        if (num_remaining == 0)
            return;

        // filter on input num_pins (TODO: binary search would probably be fastest? -> possibly)
        auto component_it = remaining_components.begin();
        for (size_t i = 0; i < num_remaining; i++) {
            auto curr_component = *component_it;

            if (curr_component.first == next_num_pins) {
                // next will alwyas exist b/c component_it only becomes it.end() at the END of the final loop 
                auto component_next_it = std::next(component_it);
                remaining_components.splice(remaining_components.end(), remaining_components, component_it);

                // Recursively compute strength using a smaller set of remaining components -> only uses O(n^2)
                // memory, where n ~= 50, so we're doing good!
                int out_length = 0;
                int out_strength = 0;
                longest_strongest_bridge(
                    remaining_components,
                    num_remaining - 1,
                    curr_component.second,
                    out_length,
                    out_strength
                );
                out_strength += curr_component.first + curr_component.second;
                out_length += 1;

                if (out_length > longest) {
                    longest = out_length;
                    strongest = out_strength;
                } else if (out_length == longest && out_strength >= strongest) {
                    strongest = out_strength;
                }

                // NOTE: b/c of this insert, the order should be fine!
                remaining_components.splice(component_next_it, remaining_components, component_it);
            }

            // NOTE: ports are allowed to be backwards! try all the ports again, but backwards!
            if (
                curr_component.second == next_num_pins
                // NOTE: adding this case tripled performance!
                && curr_component.first != curr_component.second
            ) {
                auto component_next_it = std::next(component_it);
                remaining_components.splice(remaining_components.end(), remaining_components, component_it);

                int out_length = 0;
                int out_strength = 0;
                longest_strongest_bridge(
                    remaining_components,
                    num_remaining - 1,
                    curr_component.first,
                    out_length,
                    out_strength
                );
                out_strength += curr_component.first + curr_component.second;
                out_length += 1;

                if (out_length > longest) {
                    longest = out_length;
                    strongest = out_strength;
                } else if (out_length == longest && out_strength >= strongest) {
                    strongest = out_strength;
                }

                remaining_components.splice(component_next_it, remaining_components, component_it);
            }

            component_it++;
        }
    }
public:
    void part1(const list_pairs& components) const {
        // construct ll
        std::list<std::pair<int,int>> mut_components;
        for (auto comp : components)
            mut_components.push_back(comp);

        std::cout << "(part1) bridge_strength: " << bridge_strength(mut_components, mut_components.size(), 0) << std::endl;
    }

    void part2(const list_pairs& components) const {
        std::list<std::pair<int,int>> mut_components;
        for (auto comp : components)
            mut_components.push_back(comp);

        int longest = 0;
        int strongest = 0;

        longest_strongest_bridge(
            mut_components,
            mut_components.size(),
            0,
            longest,
            strongest
        );

        std::cout << "(part2) longest_strongest: " << strongest << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day24_input.txt");
    Day24FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day24Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}