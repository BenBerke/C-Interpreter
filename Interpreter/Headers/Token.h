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

    WORD,
    NUMBER,
    CHAR_LIT,

    PRINT,
    INT,
    CHAR,

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