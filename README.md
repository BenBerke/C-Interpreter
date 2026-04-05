# C Interpreter

A small interpreter written in **C** for a custom scripting language.

I built this project to better understand how programming languages work at a lower level by implementing the core pieces myself: **lexing**, **parsing**, **AST construction**, and **interpreting**.

It currently supports:

- integer variables
- character variables
- boolean variables
- arithmetic expressions
- comparison operators
- variable assignment
- `print`
- `if` statements
- block scope
- user-defined functions
- recursive function calls
- `return`

## Why I built it

My goal was not just to use programming languages, but to understand what happens underneath them.

Building this interpreter gave me hands-on experience with:

- tokenising source code
- recursive descent parsing
- abstract syntax trees
- expression evaluation
- scope handling
- function calls and recursion
- runtime behaviour and error handling

## Example

Here is a small example script that runs recursive Fibonacci logic:

```txt
int limit = 50;
char label = 'F';
bool run = true;

function fib(a, b) {
    print a;
    int next = a + b;
    bool keepGoing = next < limit;

    if (keepGoing) {
        fib(b, next);
    }
}

print label;

if (run) {
    print '>';
    fib(0, 1);
}
```

## How it works

The interpreter follows a simple pipeline:

1. **Lexer**  
   Reads the source file and converts it into a list of tokens.

2. **Parser**  
   Builds an **Abstract Syntax Tree (AST)** from those tokens using recursive descent parsing.

3. **Interpreter**  
   Walks through the AST and executes the program, handling:
   - variable definitions
   - expressions
   - function calls
   - recursion
   - scoped blocks
   - return values

## Supported language features

### Variables

```txt
int number = 10;
char letter = 'A';
bool active = true;
```

### Arithmetic

```txt
int result = (5 + 3) * 2;
print result;
```

### Comparisons

```txt
bool isLarge = result > 10;
print isLarge;
```

### Conditionals

```txt
if (isLarge) {
    print '!';
}
```

### Functions

```txt
function add(a, b) {
    return a + b;
}

print add(3, 4);
```

### Recursion

```txt
function fib(n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

print fib(10);
```

## Project structure

The codebase is split into the main components of a small interpreter:

- **lexer** – turns source code into tokens
- **parser** – builds the AST
- **AST definitions** – expression and statement node structures
- **interpreter** – executes the parsed program
- **runtime / scope handling** – stores variables and functions during execution

## Running the project

The interpreter reads source code from a text file and executes it.

Typical workflow:

1. Write your program in `source.txt`
2. Compile the C files
3. Run the executable
4. The interpreter tokenises, parses, and executes the script

## Example output

For the Fibonacci-style example above, the interpreter prints a sequence generated through recursive calls until the limit is reached.

## Notes

This is still a small project, but it has been one of the most useful ways for me to learn about language implementation in C.

If you are interested in interpreters, compilers, or low-level programming, this project was a really fun way to explore those ideas in practice.
