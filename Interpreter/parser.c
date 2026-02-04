#include <stdio.h>
#include <stdlib.h>

#include "Headers/Parser.h"
#include "Headers/Token.h"

struct Parser {
    const Token *tokens;
    int count;
    int current;
};

typedef enum {
    BINARY,
    LITERAL
} ExprType;

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
    } as;
};

static const Token *peek(Parser *p)     { return &p->tokens[p->current]; }
static const Token *previous(Parser *p) { return &p->tokens[p->current - 1]; }
static int is_at_end(Parser *p)         { return p->current >= p->count || p->tokens[p->current].type == EF;}
static const Token *advance(Parser *p)  { if (!is_at_end(p)) p->current++; return previous(p); }

void print_list(const Token* tokenList, const int listLength) {
    for (int i = 0; i < listLength; i++) {
        if (tokenList[i].type == PRINT) printf("PRINT");
        else if (tokenList[i].type == VAR) printf("VAR");
        else if (tokenList[i].type == NUMBER) printf("NUMBER(%d)", tokenList[i].literal.i_value);
        else if (tokenList[i].type == PLUS) printf("PLUS");
        else if (tokenList[i].type == MINUS) printf("MINUS");
        else if (tokenList[i].type == SEMICOLON) printf("SEMICOLON");
        else if (tokenList[i].type == STAR) printf("STAR");
        else if (tokenList[i].type == EF) printf("EOF");
        else if (tokenList[i].type == SLASH) printf("SLASH");
        printf("\n");
    }
}

Expression* factor(Parser *p) {
    if (peek(p)->type == NUMBER) {
        Expression *expr = malloc(sizeof(struct Expression));
        expr->type = LITERAL;
        expr->as.literal.value = advance(p);
        return expr;
    }
    return NULL;
}

Expression* multiplication(Parser *p) {
    Expression *expr = factor(p);
    while (peek(p)->type == STAR || peek(p)->type == SLASH) {
        const Token* op = advance(p);
        Expression *right = factor(p);
        Expression *new_node = malloc(sizeof(struct Expression));
        new_node->type = BINARY;
        new_node->as.binary.left = expr;
        new_node->as.binary.op = op;
        new_node->as.binary.right = right;
        expr = new_node;
    }
    return expr;
}

Expression* addition(Parser *p) {
    Expression *expr = multiplication(p);
    while (peek(p)->type == PLUS || peek(p)->type == MINUS) {
        const Token* op = advance(p);
        Expression *right = multiplication(p);
        Expression *new_node = malloc(sizeof(struct Expression));
        new_node->type = BINARY;
        new_node->as.binary.left = expr;
        new_node->as.binary.op = op;
        new_node->as.binary.right = right;
        expr = new_node;
    }
    return expr;
}

Expression* parse(Parser *p) {
    Expression* expr = addition(p);
    if (peek(p)->type == SEMICOLON) advance(p);
    else if (peek(p)->type != EF) perror("; expected after an expression");
    return expr;
}

int evaluate(const Expression *expr) {
    if (expr == NULL) return 0;
    if (expr->type == LITERAL) return expr->as.literal.value->literal.i_value;
    if (expr->type == BINARY) {
        const int left = evaluate(expr->as.binary.left);
        const int right = evaluate(expr->as.binary.right);
        switch (expr->as.binary.op->type) {
            case PLUS: return left + right;
            case MINUS: return left - right;
            case STAR:  return left * right;
            case SLASH:  return left / right;
            default: return 0;
        }
    }
    return 0;
}

void free_expression(Expression *expr) {
    if (expr == NULL) return;
    if (expr->type == BINARY) {
        free(expr->as.binary.left);
        free(expr->as.binary.right);
    }
    free(expr);
}

void init_parser(const Token* tokenList, const int listLength) {
    Parser p = {tokenList, listLength, 0};
    while (!is_at_end(&p)) {
        Expression *tree = parse(&p);
        if (tree != NULL) {
            int result = evaluate(tree);
            printf("%d", result);
            free_expression(tree);
        }
    }
}