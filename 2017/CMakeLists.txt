cmake_minimum_required(VERSION 3.20)
project(AdventOfCode_2017 VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

if (!MSVC)
    add_compile_options(-Wall)
endif()

#if (MSVC)
#    set(CMAKE_C_FLAGS_RELEASE "/O2 /Zi /DNDEBUG")
#    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG /PROFILE")
#endif()

# ------------------------------------------------ #
# days

add_executable(day1 day1.cpp)
add_executable(day2 day2.cpp)
# target_link_libraries(day2 m) # only for unix?
add_executable(day3 day3.cpp)
add_executable(day4 day4.cpp)
add_executable(day5 day5.cpp file_input.cpp)
add_executable(day6 day6.cpp file_input.cpp)
add_executable(day7 day7.cpp file_input.cpp)
add_executable(day8 day8.cpp file_input.cpp)
add_executable(day9 day9.cpp file_input.cpp)
add_executable(day10
    day10.cpp day10_solution.cpp file_input.cpp
)
add_executable(day11 day11.cpp file_input.cpp)
add_executable(day12 day12.cpp file_input.cpp)
add_executable(day13 day13.cpp file_input.cpp)
add_executable(day14
    day14.cpp day10_solution.cpp file_input.cpp
)
add_executable(day15 day15.cpp file_input.cpp)
add_executable(day16 day16.cpp)
add_executable(day17 day17.cpp)
add_executable(day18 day18.cpp)
add_executable(day19 day19.cpp)
add_executable(day20 day20.cpp)
add_executable(day21 day21.cpp)
add_executable(day22 day22.cpp)
add_executable(day23 day23.cpp)
add_executable(day24 day24.cpp)
add_executable(day25 day25.cpp)

# ------------------------------------------------ #