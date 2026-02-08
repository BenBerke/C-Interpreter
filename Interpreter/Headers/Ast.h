#ifndef AST_H
#define AST_H
#include "Token.h"

typedef struct Parser Parser;
typedef struct Expression Expression;
typedef struct Statement Statement;
typedef struct Program Program;

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_CHAR,
} VarType;

typedef struct {
    char* name;
    VarType type;
    union {
        int i_value;
        char* s_value;
        char* c_value;
    } data;
} Variable;

typedef enum {
    BINARY,
    LITERAL,
    VAR_EXPR,
} ExprType;

typedef enum {
    STMT_EXPRESSION,
    STMT_PRINT,
    STMT_ASSIGN,
    STMT_CREATE_INT,
    STMT_CREATE_CHAR,
} StmtType;

struct Statement {
    StmtType type;
    union {
        struct Expression* expression;
        struct {
            const Token* name;
            struct Expression* value;
        } Assignment;
    } as;
};

struct Expression {
    ExprType type;
    union {
        struct {
            struct Expression *left;
            const Token* op;
            struct Expression *right;
        } binary;

        struct {
            const Token* value;
        } literal;

        struct {
            const Token* name;
        }variable;
    } as;
};

struct Program {
    Statement** statements;
    int count;
};

#endif
