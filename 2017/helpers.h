#ifndef HELPERS_H
#define HELPERS_H

#include <array>
#include <iostream>
#include <format>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

template <typename T, size_t Dimensions>
class Matrix {
private:
    std::vector<T> data;
    std::array<size_t, Dimensions> dimensions;

    template <typename... Dims>
        requires (sizeof...(Dims) == Dimensions)
                 && (std::conjunction_v<std::is_convertible<Dims, size_t>...>)
    size_t get_index(Dims... coords) const noexcept {
        size_t index = 0;
        size_t factor = data.size();
        size_t i = 0;

        ([&] {
            factor /= this->dimensions[i];
            index += factor * coords;
            i += 1;
        // note that we're folding over the comma operator here, which sequences
        // lambda calls in-order
        }(), ...);
        return index;
    }

public:
    template <typename... Dims>
        // checked at compile (type substitution) time! Don't make it an error since another valid overload may exist
        requires (sizeof...(Dims) == Dimensions)
                 && (std::conjunction_v<std::is_convertible<Dims, size_t>...>)
    Matrix(Dims... dim) : data((dim * ...)), dimensions{dim...} {
        // sanity check
        std::cout << std::format("size of dimensions = {}", dimensions.size()) << std::endl;
        for (size_t d : dimensions) {
            if (d == 0)
                throw std::runtime_error("all dimensions must be non-zero");
        }
    }
    
    template <typename... Dims>
        requires (sizeof...(Dims) == Dimensions)
                 && (std::conjunction_v<std::is_convertible<Dims, size_t>...>)
    T& get(Dims... coords) noexcept {
        return data[this->get_index(coords...)];
    }

    template <typename... Dims>
        requires (sizeof...(Dims) == Dimensions)
                 && (std::conjunction_v<std::is_convertible<Dims, size_t>...>)
    const T& get_const(Dims... coords) const noexcept {
        return data[this->get_index(coords...)];
    }

    const std::array<size_t, Dimensions>& size() const noexcept {
        return this->dimensions;
    }

    // TODO: enable getting a lower dimensional mutable reference slice
    // of the original object
};

#endif