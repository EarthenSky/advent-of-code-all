#ifndef DAY_10_H
#define DAY_10_H

#include <iterator>
#include <string>
#include <vector>

#include <cstdint>

#include "abstract_solution.h"

class Day10Solution : ISolution<std::vector<size_t>> {
private:
    template <typename Iter>
    requires std::input_iterator<Iter>
    static void reverse_contiguous_iterators(Iter begin_a, Iter end_a, Iter begin_b, Iter end_b);

public:
    void part1(const std::vector<size_t>& length_sequence) const;
    void part2(const std::vector<size_t>& length_sequence) const;

    std::array<uint8_t, 16> compute_hash(std::string input_string) const;
};


#endif