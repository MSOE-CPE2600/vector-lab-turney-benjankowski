cmake_minimum_required(VERSION 3.15)
project(lab_5 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(lab_5.elf
        src/main.c
        src/vectors.c
        src/equate.c
)

target_compile_options(lab_5.elf PRIVATE -Wall)
target_link_libraries(lab_5.elf m)
