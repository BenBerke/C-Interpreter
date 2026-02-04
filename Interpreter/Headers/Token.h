#ifndef TOKENS_H
#define TOKENS_H
typedef enum {
    EQL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    SEMICOLON,

    NUMBER,

    PRINT,
    VAR,

    EF,
} Type;

typedef struct {
    Type type;
    union {
        int i_value;
        char *s_value;
    } literal;
} Token;

#endif