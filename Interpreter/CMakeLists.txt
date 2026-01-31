cmake_minimum_required(VERSION 4.1)
project(CLion_Projects C)

set(CMAKE_C_STANDARD 11)

add_executable(CLion_Projects lexer.c
        Headers/Tokens.h
)
