cmake_minimum_required(VERSION 3.25)
project(AdventOfCode
    VERSION 0.1
    LANGUAGES C
)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(geblib_c)

#add_subdirectory(2015)
#add_subdirectory(2016)
#add_subdirectory(2017)
add_subdirectory(2025)
