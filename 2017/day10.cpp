#include <algorithm>
#include <format>
#include <fstream>
#include <ranges>

#include <cassert>

#include "abstract_solution.h"
#include "file_input.h"

class Day10FileParser : IFileParser<std::vector<size_t>> {
public:
    using IFileParser<std::vector<size_t>>::IFileParser;

    std::vector<size_t> parse() const {
        std::ifstream input_file(this->file_path);
        
        std::string line;
        std::getline(input_file, line);

        std::vector<size_t> numbers;
        for (auto&& item : std::views::split(line, ',')) {
            numbers.push_back(
                (size_t) std::stoi(std::string(item.begin(), item.end()))
            );
        }

        return numbers;
    }
};

class Day10Solution : ISolution<std::vector<size_t>> {
private:
    template <typename Iter>
    static void reverse_contiguous_iterators(Iter begin_a, Iter end_a, Iter begin_b, Iter end_b) {
        // using ValueType = typename std::iterator_traits<Iter>::value_type;

        size_t length_a = end_a - begin_a;
        size_t length_b = end_b - begin_b;

        assert(length_a != 0);
        assert(length_b != 0);

        if (length_a == length_b) {
            std::swap_ranges(begin_a, end_a, begin_b);
            std::reverse(begin_a, end_a);
            std::reverse(begin_b, end_b);
        } else if (length_a > length_b) {
            // [a .. a+blen] [a+blen .. a+alen] [b .. b+blen]
            std::swap_ranges(begin_a, begin_a + length_b, begin_b);
            std::reverse(begin_a, begin_a + length_b);
            std::reverse(begin_b, end_b);
            std::reverse(begin_a + length_b, end_a);
        } else {
            std::swap_ranges(begin_b + (length_b - length_a), end_b, begin_a);
            std::reverse(begin_b + (length_b - length_a), end_b);
            std::reverse(begin_a, end_a);
            std::reverse(begin_b, begin_b + (length_b - length_a));
        }
    }

public:
    void part1(const std::vector<size_t>& length_sequence) const {
        std::array<uint8_t, 256> bytes;
        for (size_t i = 0; i < bytes.size(); i++)
            bytes[i] = i;

        size_t cursor_location = 0;
        size_t skip_size = 0;
        for (size_t length : length_sequence) {
            if (cursor_location + length <= bytes.size()) {
                std::reverse(bytes.begin() + cursor_location, bytes.begin() + cursor_location + length);
            } else {
                size_t len_second = (cursor_location + length) % bytes.size();
                Day10Solution::reverse_contiguous_iterators(
                    bytes.begin() + cursor_location, bytes.end(),
                    bytes.begin(), bytes.begin() + len_second
                );
            }

            cursor_location = (cursor_location + length + skip_size) % bytes.size();
            skip_size += 1;
        }

        std::cout << std::format("(part1) product of first 2 elements: {}", (size_t)bytes[0] * (size_t)bytes[1]) << std::endl;
    };
    void part2(const std::vector<size_t>& length_sequence) const {
        // convert from numbers into comma separated ascii
        std::vector<size_t> length_sequence_from_csv_ascii;
        for (size_t length : length_sequence) {
            for (char c : std::to_string(length))
                length_sequence_from_csv_ascii.push_back((size_t)c);
            length_sequence_from_csv_ascii.push_back(44);
        }
        length_sequence_from_csv_ascii.pop_back(); // remove trailing comma
        length_sequence_from_csv_ascii.insert(length_sequence_from_csv_ascii.end(), {17, 31, 73, 47, 23});

        std::array<uint8_t, 256> bytes;
        for (size_t i = 0; i < bytes.size(); i++)
            bytes[i] = i;

        size_t cursor_location = 0;
        size_t skip_size = 0;

        const size_t NUM_ROUNDS = 64;
        for (size_t round_i = 0; round_i < NUM_ROUNDS; round_i++) {
            // single round
            for (size_t length : length_sequence_from_csv_ascii) {
                if (cursor_location + length <= bytes.size()) {
                    std::reverse(bytes.begin() + cursor_location, bytes.begin() + cursor_location + length);
                } else {
                    size_t len_second = (cursor_location + length) % bytes.size();
                    Day10Solution::reverse_contiguous_iterators(
                        bytes.begin() + cursor_location, bytes.end(),
                        bytes.begin(), bytes.begin() + len_second
                    );
                }

                cursor_location = (cursor_location + length + skip_size) % bytes.size();
                skip_size += 1;
            }

        }

        std::cout << "(part2) dense hash: ";

        // now, convert from sparse hash to a dense hash!
        std::array<uint8_t, 16> dense_hash;
        for (size_t i = 0; i < bytes.size() / 16; i++) {
            dense_hash[i] = bytes[i * 16];
            for (size_t block_i = 1; block_i < 16; block_i++) {
                dense_hash[i] ^= bytes[i * 16 + block_i];
            }
            std::cout << std::format("{:02x}", dense_hash[i]);
        }

        std::cout << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day10_input.txt");
    Day10FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day10Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}