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

    WORD,
    NUMBER,
    CHAR_LIT,

    PRINT,
    INT,
    CHAR,

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