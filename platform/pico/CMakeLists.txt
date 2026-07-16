cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(chip8_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(chip8_pico
    main.c
)
target_link_libraries(chip8_pico pico_stdlib)
pico_add_extra_outputs(chip8_pico)