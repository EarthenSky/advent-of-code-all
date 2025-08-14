#ifndef ABSTRACT_SOLUTION_H
#define ABSTRACT_SOLUTION_H

// this doesn't actually do anything useful, I just thought it was good practice
template <typename T>
class ISolution {
public:
    virtual void part1(const T&) const = 0;
    virtual void part2(const T&) const = 0;
};

#endif