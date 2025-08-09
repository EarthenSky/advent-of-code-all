#ifndef ABSTRACT_SOLUTION_H
#define ABSTRACT_SOLUTION_H

template <typename T>
class Solution {
public:
    virtual void part1(const T&) const;
    virtual void part2(const T&) const;
};

#endif