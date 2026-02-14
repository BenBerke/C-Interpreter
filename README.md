# C Programming Language Interpreter (Toy Language)

A **from-scratch interpreter in C** demonstrating core language tooling: **lexical analysis, parsing to an AST, and tree-walking evaluation**. Built as a portfolio project to deepen low-level skills (manual memory management, data structures, and runtime design).

## What it does
- Reads `source.txt`, tokenises input into a token stream
- Parses statements/expressions with a **recursive-descent parser** and builds an **AST**
- Executes the program via a **tree-walking interpreter**
- Stores variables in a simple runtime environment (name → value)

## Language features (current)
**Statements**
- `int name = expr;` (declaration)
- `name = expr;` (assignment)
- `print expr;` (output)
- `expr;` (expression statement)

**Expressions**
- Integer literals (`123`)
- Variable references (`x`)
- Parentheses (`( ... )`)
- Binary operators: `+ - * /`
- Correct operator precedence (`* /` before `+ -`)

## Example
`source.txt`
```c
int x = 5 + 2 * 3;
print x;
x = x - 4;
print x;
'''
Output
11
7

## Build and Run
gcc -std=c11 -O2 -Wall -Wextra -pedantic \
  main.c Parser.c Interpreter.c \
  -o interpreter

./interpreter

## Technical highlights 

Implemented a lexer/tokeniser handling keywords, identifiers, numbers, operators, parentheses, and char literals

Built a recursive-descent parser with precedence levels (factor → multiplication → addition)

Designed an AST for expressions and statements and wrote a tree-walking evaluator

Implemented basic runtime state (variable table) and explicit AST memory cleanup (free_expression, free_statement)
