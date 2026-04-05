#ifndef TOKENS_H
#define TOKENS_H
typedef enum {
    EQUAL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    SEMICOLON,
    LEFT_PAR,
    RIGHT_PAR,
    COMMA,
    LEFT_BRACE,
    RIGHT_BRACE,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    EQUAL_EQUAL,

    WORD,
    NUMBER,
    CHAR_LIT,

    PRINT,
    INT,
    BOOL,
    CHAR,
    IF,
    RETURN,

    FUNCTION,

    EF,
} Type;

typedef struct {
    char* name;
    Type type;
    union {
        int i_value;
        char c_value;
        char *s_value;
    } literal;
} Token;

#endif