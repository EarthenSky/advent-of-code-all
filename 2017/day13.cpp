#include <algorithm>
#include <format>
#include <fstream>
#include <numeric>
#include <ranges>
#include <unordered_set>

#include <cassert>

#include "abstract_solution.h"
#include "file_input.h"

typedef std::vector<std::pair<size_t, size_t>> pair_vector;

class Day13FileParser : IFileParser<pair_vector> {
public:
    using IFileParser<pair_vector>::IFileParser;

    pair_vector parse() const {
        std::ifstream input_file(this->file_path);
        pair_vector layers;

        std::string line;
        while (std::getline(input_file, line)) {
            auto it = std::views::split(line, std::string(": ")).begin();
            std::string layer_str((*it).begin(), (*it).end());
            auto it2 = std::ranges::next(it, 1);
            std::string depth_str((*it2).begin(), (*it2).end());

            size_t layer = std::stoi(layer_str);
            size_t depth = std::stoi(depth_str);

            layers.push_back(std::make_pair(layer, depth));
        }

        return layers;
    }
};

class Day13Solution : ISolution<pair_vector> {
public:
    void part1(const pair_vector& layers) const {
        size_t total_severity = 0;

        // assume layers is sorted in order of layers
        for (auto [layer, range] : layers) {
            if (layer % (2 * range - 2) == 0) {
                total_severity += layer * range;
            }
        }

        std::cout << std::format("(part1) total_severity: {}", total_severity) << std::endl;        
    }

    void part2(const pair_vector& layers) const {
        std::unordered_set<size_t> offsets; // ps offsets
        offsets.insert(0);
        size_t combined_range = 1;

        for (auto [layer, range] : layers) {
            size_t current_range = 2 * range - 2;
            size_t offset = layer % current_range;
            
            // equal to x % current_range != (0-offset) % current_range
            std::cout << std::format("(x+{}) % {} != 0", offset, current_range) << std::endl;

            if (combined_range % current_range == 0) {
                assert(current_range <= combined_range);
                assert(std::gcd(combined_range, current_range) == current_range);

                // I GOT IT BACKWARDS?
                // (x + layer0) % 33 != 0
                // (x + layer1) % 11 != 0
                // =>
                // x % 33 != { (-layer0), (-layer1)+11*i}
                auto filter_view = std::views::filter(offsets, [current_range, offset](size_t item) {
                    return (item % current_range) != (
                        (current_range - offset) % current_range
                    );
                });
                offsets = std::unordered_set<size_t>(filter_view.begin(), filter_view.end());
            } else {
                std::unordered_set<size_t> new_offsets;
                size_t new_factor = current_range / std::gcd(combined_range, current_range);
                for (size_t old_offset : offsets) {
                    for (size_t i = 0; i < new_factor; i++) {
                        new_offsets.insert(old_offset + i * combined_range);
                    }
                }
                size_t old_factor = combined_range / std::gcd(combined_range, current_range);
                auto filter_view = std::views::filter(new_offsets, [current_range, offset](size_t item) {
                    return (item % current_range) != (
                        (current_range - offset) % current_range
                    );
                });
                new_offsets = std::unordered_set<size_t>(filter_view.begin(), filter_view.end());
                combined_range *= new_factor;
                offsets = std::move(new_offsets);
            }
        }

        if (offsets.empty())
            throw std::runtime_error("No solution!");

        std::cout << std::format(
            "(part2) smallest_offset: {}",
            *std::min_element(offsets.begin(), offsets.end())
        ) << std::endl;   
    }
};

int main() {
    std::filesystem::path input_path("./day13_input.txt");
    Day13FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day13Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}