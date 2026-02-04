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

typedef enum {
    STMT_EXPRESSION,
    STMT_PRINT
} StmtType;

struct Statement {
    StmtType type;
    union {
        struct Expression* expression;
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
    } as;
};

struct Program {
    Statement** statements;
    int count;
};

static const Token *peek(Parser *p)     { return &p->tokens[p->current]; }
static const Token *previous(Parser *p) { return &p->tokens[p->current - 1]; }
static int is_at_end(Parser *p)         { return p->current >= p->count || p->tokens[p->current].type == EF;}
static const Token *consume(Parser *p)  { if (!is_at_end(p)) p->current++; return previous(p); }

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
        else if (tokenList[i].type == LEFT_PAR) printf("LEFT PARENTHESIS");
        else if (tokenList[i].type == RIGHT_PAR) printf("RIGHT PARENTHESIS");
        printf("\n");
    }
}

Expression* factor(Parser *p) {
    if (peek(p)->type == NUMBER) {
        Expression *expr = malloc(sizeof(struct Expression));
        expr->type = LITERAL;
        expr->as.literal.value = consume(p);
        return expr;
    }
    return NULL;
}

Expression* multiplication(Parser *p) {
    Expression *expr = factor(p);
    while (peek(p)->type == STAR || peek(p)->type == SLASH) {
        const Token* op = consume(p);
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
        const Token* op = consume(p);
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

Statement* parse(Parser *p) {
    Statement* stmt = malloc(sizeof(struct Statement));
    Expression* expr = NULL;
    if (peek(p)->type == PRINT) {
        consume(p);
        expr = addition(p);
        stmt->type = STMT_PRINT;
    } else {
        expr = addition(p);
        stmt->type = STMT_EXPRESSION;
    }
    stmt->as.expression = expr;
    if (peek(p)->type == SEMICOLON) consume(p);
    else if (peek(p)->type != EF) perror("; expected after an expression");
    return stmt;
}

int evaluate_expression(Expression *expr) {
    if (expr == NULL) return 0;
    if (expr->type == LITERAL) return expr->as.literal.value->literal.i_value;
    if (expr->type == BINARY) {
        const int left = evaluate_expression(expr->as.binary.left);
        const int right = evaluate_expression(expr->as.binary.right);
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

int evaluate_statement(const Statement *stmt) {
    if (stmt == NULL) return 0;
    if (stmt->type == STMT_EXPRESSION) return evaluate_expression(stmt->as.expression);
    if (stmt->type == STMT_PRINT) printf("%d\n", evaluate_expression(stmt->as.expression));

    return 0;
}

void free_expression(Expression *expr) {
    if (expr == NULL) return;
    if (expr->type == BINARY) {
        free_expression(expr->as.binary.left);
        free_expression(expr->as.binary.right);
    }
    free(expr);
}

void free_statement(Statement *stmt) {
    if (stmt == NULL) return;
    if (stmt->as.expression != NULL) free_expression(stmt->as.expression);
    free(stmt);
}

void init_parser(const Token* tokenList, const int listLength) {
    Parser p = {tokenList, listLength, 0};
    Program program;
    program.count = 0;
    program.statements = NULL;
    while (!is_at_end(&p)) {
        Statement *tree = parse(&p);
        program.count++;
        program.statements = realloc(program.statements, program.count * sizeof(Statement*));
        program.statements[program.count - 1] = tree;
    }
    for (int i = 0; i < program.count; i++) evaluate_statement(program.statements[i]);
    for (int i = 0; i < program.count; i++) free_statement(program.statements[i]);
    free(program.statements);
}