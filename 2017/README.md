# advent of code 2017 - solutions
- in C++20 (woo!)

## setup
```sh
cmake -S . -B build
cmake --build build
# printf "add data here" > input_day1.txt
./build/day1
```

## additional notes
- Only tested on WSL (debian)
- Ensure you have GCC 13 or newer, since we use std::format for string concats <3
- Ensure input files are LF
