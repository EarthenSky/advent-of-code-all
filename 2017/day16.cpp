#include <algorithm>
#include <fstream>
#include <format>
#include <functional>
#include <ranges>
#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

class Day16FileParser : IFileParser<std::vector<std::string>> {
public:
    using IFileParser<std::vector<std::string>>::IFileParser;

    std::vector<std::string> parse() const {
        std::ifstream input_file(this->file_path);
        std::string line;
        std::getline(input_file, line);

        std::vector<std::string> moves;
        for (const auto& split_view : std::views::split(line, ',')) {
            std::string dance_move(split_view.begin(), split_view.end());
            moves.push_back(dance_move);
        }

        return moves;
    }
};

class Day16Solution {
private:
    const size_t NUM_DANCERS = 16;

    void apply_move(std::vector<char>& dancers, const std::string& dance_move) const {
        // TODO: update dancers
        if (dance_move[0] == 's') {
            std::vector<char> new_dancers(dancers.size());
            int spin_size;
            std::from_chars(dance_move.data() + 1, dance_move.data() + dance_move.size(), spin_size);
            std::copy(dancers.end() - spin_size, dancers.end(), new_dancers.begin());
            std::copy(dancers.begin(), dancers.end() - spin_size, new_dancers.begin() + spin_size);
            dancers = std::move(new_dancers);
        } else if (dance_move[0] == 'x') {
            int first_i, second_i;
            auto result = std::from_chars(dance_move.data() + 1, dance_move.data() + 1 + 2, first_i);
            std::from_chars(result.ptr + 1, dance_move.data() + dance_move.size(), second_i);
            //std::cout << "x" << first_i << "/" << second_i << std::endl;
            std::swap(dancers[first_i], dancers[second_i]);
        } else if (dance_move[0] == 'p') {
            size_t first_i = std::find(dancers.begin(), dancers.end(), dance_move[1]) - dancers.begin();
            size_t second_i = std::find(dancers.begin(), dancers.end(), dance_move[3]) - dancers.begin();
            std::swap(dancers[first_i], dancers[second_i]);
        } else {
            throw std::runtime_error("got invalid dance move!");
        }
    }
public:
    std::vector<char> part1(const std::vector<std::string>& moves) const {
        std::vector<char> dancers(NUM_DANCERS);
        for (size_t i = 0; i < dancers.size(); i++) {
            dancers[i] = 'a' + i;
        }

        for (auto move : moves)
            apply_move(dancers, move);

        std::cout << "(part1) dancers: ";
        for (char dancer : dancers) {
            std::cout << dancer;
        }
        std::cout << std::endl;

        return dancers;
    }

    /// @brief update dancers using the swizzle from abcdef... to $dancers_after
    // TODO: will this be returned as a temporary?
    std::vector<char> apply_dance_swizzle(const std::vector<char>& dancers, const std::vector<char>& dancers_after) const {
        std::vector<char> new_dancers;
        for (char c : dancers_after) {
            new_dancers.push_back(dancers[c - 'a']);
        }
        return new_dancers;
    }

    void part2(const std::vector<std::string>& moves, const std::vector<char>& dancers_after_1_time) {
        const size_t ONE_BILLION = 1000 * 1000 * 1000;

        std::vector<char> default_dancers(NUM_DANCERS);
        for (size_t i = 0; i < default_dancers.size(); i++)
            default_dancers[i] = 'a' + i;
        
        std::vector<char> dancers(NUM_DANCERS);
        for (size_t i = 0; i < dancers.size(); i++)
            dancers[i] = 'a' + i;

        size_t time_to_repeat;
        for (size_t i = 0; i < ONE_BILLION; i++) {
            for (auto move : moves)
                apply_move(dancers, move);

            if (default_dancers == dancers) {
                time_to_repeat = i+1;
                break;
            }
        }

        // if we repeat after some number of dances, then we may simply only play the remainder!
        for (size_t i = 0; i < (ONE_BILLION % time_to_repeat); i++) {
            for (auto move : moves)
                apply_move(dancers, move);
        }

        std::cout << "(part2) dancers: ";
        for (char dancer : dancers) {
            std::cout << dancer;
        }
        std::cout << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day16_input.txt");
    Day16FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day16Solution solution;
    auto dancers = solution.part1(data);
    solution.part2(data, dancers);
    return 0;
}