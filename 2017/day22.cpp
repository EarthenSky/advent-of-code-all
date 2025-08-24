#include <algorithm>
#include <array>
#include <fstream>
#include <format>
#include <unordered_set>
//#include <ranges>
#include <regex>
//#include <cctype>
//#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

struct int_pair_hash {
    size_t operator()(const std::pair<int, int>& pair) const noexcept {
        return std::hash<int>()(pair.first) ^ (37 * std::hash<int>()(pair.second));
    }
};

struct Map {
    std::unordered_set<std::pair<int, int>, int_pair_hash> infections;
    size_t init_width;
    size_t init_height;
};

enum Direction {
    NORTH = 0, EAST, SOUTH, WEST
};

class Day22FileParser : IFileParser<Map> {
public:
    using IFileParser<Map>::IFileParser;
    
    Map parse() const {
        Map map;
        map.init_height = 0;
        map.init_width = 0;

        std::ifstream input_file(this->file_path);
        std::string line;
        int y = 0;
        while (std::getline(input_file, line)) {
            int x = 0;
            for (char ch : line) {
                if (ch == '\n') {
                    break; // eol
                } else if (ch == '#') {
                    map.infections.insert(std::make_pair(x, y));
                }
                x += 1;
            }
            map.init_width = x;
            y += 1;
        }

        map.init_height = y;
        return map;
    }
};

class Day22Solution {
    // TODO: what can we do with constexpr?
    static constexpr size_t TEN_THOUSAND = 10 * 1000;

    Direction turn_left(Direction dir) const noexcept {
        if (dir == NORTH) {
            return WEST;
        } else if (dir == EAST) {
            return NORTH;
        } else if (dir == SOUTH) {
            return EAST;
        } else {
            return SOUTH;
        }
    }
    Direction turn_right(Direction dir) const noexcept {
        if (dir == NORTH) {
            return EAST;
        } else if (dir == EAST) {
            return SOUTH;
        } else if (dir == SOUTH) {
            return WEST;
        } else {
            return NORTH;
        }
    }
public:
    void both_parts(const Map& _rules) const {

        // store nodes as hash map, due to essentially infinite grid size
        // 

        Map rules_mut = _rules;

        int x = rules_mut.init_width / 2; // 3/2 -> 1, which is exactly what we wish for! [0, 1, 2]
        int y = rules_mut.init_height / 2;

        Direction dir = NORTH;

        size_t num_infection_bursts = 0;

        for (size_t activity_i = 0; activity_i < Day22Solution::TEN_THOUSAND; activity_i++) {
            if (rules_mut.infections.contains(std::make_pair(x, y))) {
                dir = this->turn_right(dir);
                rules_mut.infections.erase(std::make_pair(x, y));
            } else {
                dir = this->turn_left(dir);
                rules_mut.infections.insert(std::make_pair(x, y));
                num_infection_bursts += 1;
            }
        
            // move forwards one step
            if (dir == NORTH) {
                y -= 1;
            } else if (dir == EAST) {
                x += 1;
            } else if (dir == SOUTH) {
                y += 1;
            } else if (dir == WEST) {
                x -= 1;
            }
        }

        std::cout << "(part1) num_infection_bursts: " << num_infection_bursts << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day22_input.txt");
    Day22FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day22Solution solution;
    solution.both_parts(data);
    return 0;
}