set(DAYS day1 day2 day3 day4 day5 day6 day7 day8 day9)

foreach(day ${DAYS})
    add_executable(${day} ${day}.c)
    set_target_properties(${day} PROPERTIES
        COMPILE_FLAGS "-Wall -Wextra -fsanitize=address -g"
        LINK_FLAGS "-fsanitize=address"
    )
    target_compile_features(${day} PRIVATE c_std_99)

    # TODO: does the linker remove dead code by default?
    # TODO: why use PRIVATE here?
    target_link_libraries(${day} PRIVATE geblib)
endforeach()

target_link_libraries(day8 PRIVATE m)
