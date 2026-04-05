cmake_minimum_required(VERSION 3.20)
project(CInterpreter C)

set(CMAKE_C_STANDARD 11)

include_directories(Headers)

add_executable(interpreter
        lexer.c
        parser.c
        Interpreter.c
        Library/Dictionary.c
        Headers/Token.h
        Headers/Parser.h
        Headers/Expression.h
        Headers/Literal.h
        Headers/Ast.h
        Headers/Interpreter.h
        Headers/Dictionary.h
)