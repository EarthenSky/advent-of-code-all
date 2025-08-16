#ifndef ABSTRACT_SOLUTION_H
#define ABSTRACT_SOLUTION_H

// The only thing this helps with is clearer typechecking
template <typename T>
class ISolution {
public:
    virtual void part1(const T&) const = 0;
    virtual void part2(const T&) const = 0;
};

#endif