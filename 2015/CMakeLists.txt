cmake_minimum_required(VERSION 3.20)
project(AdventOfCode VERSION 0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# ------------------------------------------------ #
# days

add_executable(day1 day1.c)

add_executable(day2 day2.c)
link_libraries(day2 m)

# ------------------------------------------------ #