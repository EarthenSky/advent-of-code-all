cmake_minimum_required(VERSION 3.20)
project(AdventOfCode VERSION 0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "/O2 /Zi /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /PROFILE")
endif()

# ------------------------------------------------ #
# days

add_executable(day1 day1.c)
add_executable(day2 day2.c)
# target_link_libraries(day2 m) # only for unix
add_executable(day3 day3.c)
add_executable(day4 day4.c)
add_executable(day5 day5.c)
add_executable(day6 day6.c)
add_executable(day7 day7.c)
add_executable(day8 day8.c)
add_executable(day9 day9.c)
add_executable(day10 day10.c)
add_executable(day11 day11.c)
add_executable(day12 day12.c)

# ------------------------------------------------ #