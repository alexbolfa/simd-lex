cmake_minimum_required(VERSION 3.28)
project(simd_lexer C)

set(CMAKE_C_COMPILER "clang")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbmi2")

add_executable(simd_lexer main.c
        lexer.c
        lexer.h
        tokens.h
        tokens.c
        print_utils.c
)
