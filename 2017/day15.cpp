#include <algorithm>
#include <fstream>
#include <format>
#include <functional>
#include <queue>

#include <thread>
#include <mutex>

#include "abstract_solution.h"
#include "file_input.h"

class Day15FileParser : IFileParser<std::pair<size_t, size_t>> {
public:
    using IFileParser<std::pair<size_t, size_t>>::IFileParser;

    std::pair<size_t, size_t> parse() const {
        std::ifstream input_file(this->file_path);
        std::string line_a;
        std::getline(input_file, line_a);
        std::string line_b;
        std::getline(input_file, line_b);

        const size_t BEFORE_NUMBER = 24;
        return std::make_pair(
            std::stoi(
                std::string(line_a.begin() + BEFORE_NUMBER, line_a.end())
            ), std::stoi(
                std::string(line_b.begin() + BEFORE_NUMBER, line_b.end())
            )
        );
    }
};

class Day15Solution {
    static const size_t FACTOR_A = 16807;
    static const size_t FACTOR_B = 48271;
    static const size_t FINAL_EQUIV_CLASS = 2147483647;
    static const size_t N_PAIRS = 40 * 1000 * 1000;
    static const size_t N_PAIRS_SMALLER = 5 * 1000 * 1000;

public:
    void part1(const std::pair<size_t, size_t>& starting_values) const {
        size_t total_equal_pairs = 0;
        size_t a = starting_values.first;
        size_t b = starting_values.second;
        for (size_t pair_i = 0; pair_i < N_PAIRS; pair_i++) {
            if ((a & 0xffff) == (b & 0xffff)) {
                total_equal_pairs += 1;
            }
            a = (a * FACTOR_A) % FINAL_EQUIV_CLASS;
            b = (b * FACTOR_B) % FINAL_EQUIV_CLASS;
        }

        std::cout << std::format("(part1) total_equal_pairs: {}", total_equal_pairs) << std::endl;
    }

    std::mutex a_values_lock;
    std::mutex b_values_lock;

    std::mutex a_erase_lock;
    std::mutex b_erase_lock;

    void filter_generator_a(std::vector<size_t>& items, size_t a) {
        size_t sent = 0;
        while (sent < N_PAIRS_SMALLER) {
            while (a % 4 != 0) {
                a = (a * FACTOR_A) % FINAL_EQUIV_CLASS;
            }
            if (items.size() == items.capacity()) {
                std::lock_guard lock(a_values_lock);
                std::lock_guard erase_lock(a_erase_lock);
                items.push_back(a);
            } else {
                std::lock_guard erase_lock(a_erase_lock);
                items.push_back(a);
            }

            sent += 1;
            a = (a * FACTOR_A) % FINAL_EQUIV_CLASS;
        }
    }
    void filter_generator_b(std::vector<size_t>& items, size_t b) {
        size_t sent = 0;
        while (sent < N_PAIRS_SMALLER) {
            while (b % 8 != 0) {
                b = (b * FACTOR_B) % FINAL_EQUIV_CLASS;
            }
            if (items.size() == items.capacity()) {
                std::lock_guard lock(b_values_lock);
                std::lock_guard erase_lock(b_erase_lock);
                items.push_back(b);
            } else {
                std::lock_guard erase_lock(b_erase_lock);
                // a cool trick that allows us to push items into the vector while reading from it!
                items.push_back(b);
            }
            sent += 1;
            b = (b * FACTOR_B) % FINAL_EQUIV_CLASS;
        }
    }

    // NOTE: the muktithreaded version is slower because we're passing so much data around!
    // ALSO, the overhead from the locks is probably pretty large ;-;
    void part2(const std::pair<size_t, size_t>& starting_values) {
        std::vector<size_t> items_a;
        std::vector<size_t> items_b;

        size_t a = starting_values.first;
        size_t b = starting_values.second;
        std::jthread generator_a(&Day15Solution::filter_generator_a, this, std::ref(items_a), a);
        std::jthread generator_b(&Day15Solution::filter_generator_b, this, std::ref(items_b), b);

        size_t total_equal_pairs = 0;
        size_t n_consumed_pairs = 0;
        while (n_consumed_pairs < N_PAIRS_SMALLER) {
            size_t min_shared;
            {
                std::lock_guard lock_a(a_values_lock);
                std::lock_guard lock_b(b_values_lock);
                
                min_shared = std::min(items_a.size(), items_b.size());
                for (size_t i = 0; i < min_shared; i++) {
                    if ((items_a[i] & 0xffff) == (items_b[i] & 0xffff)) {
                        total_equal_pairs += 1;
                    }
                }
            }

            {
                // erase lock stops a push and an erase to occur at the same time
                std::lock_guard erase_lock_a(a_erase_lock);
                std::lock_guard erase_lock_b(b_erase_lock);

                // erase will never shrink capacity, so we're good...?
                items_a.erase(items_a.begin(), items_a.begin() + min_shared);
                items_b.erase(items_b.begin(), items_b.begin() + min_shared);
            }
            n_consumed_pairs += min_shared;
            // std::cout << "consumed " << n_consumed_pairs << std::endl;

            std::this_thread::sleep_for(std::chrono::duration<size_t, std::milli>(5));
        }

        std::cout << std::format("(part2) total_equal_pairs: {}", total_equal_pairs) << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day15_input.txt");
    Day15FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day15Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}