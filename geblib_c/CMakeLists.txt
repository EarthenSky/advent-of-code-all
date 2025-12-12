set(SRC_FILES
    "src/io.c"
)

add_library(geblib ${SRC_FILES})
set_target_properties(geblib PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra"
)

target_include_directories(geblib PUBLIC include)
target_compile_features(geblib PRIVATE c_std_99)
