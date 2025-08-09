cmake_minimum_required(VERSION 3.20)
project(AdventOfCode_2017 VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

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
#add_executable(day6 day6.cpp)
#add_executable(day7 day7.cpp)
#add_executable(day8 day8.cpp)
#add_executable(day9 day9.cpp)


# ------------------------------------------------ #