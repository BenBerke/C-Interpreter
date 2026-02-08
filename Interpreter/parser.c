#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Headers/Parser.h"
#include "Headers/Token.h"
#include "Headers/Ast.h"
#include "Headers/Interpreter.h"

Expression* addition(Parser *p);
Expression* multiplication(Parser *p);

struct Parser {
    const Token *tokens;
    int count;
    int current;
};

Token** variables = NULL;
int variableCount = 0;

static const Token *peek(Parser *p)     { return &p->tokens[p->current]; }
static const Token *previous(Parser *p) { return &p->tokens[p->current - 1]; }
static int is_at_end(Parser *p)         { return p->current >= p->count || p->tokens[p->current].type == EF;}
static const Token *consume(Parser *p)  { if (!is_at_end(p)) p->current++; return previous(p); }

void print_list(const Token* tokenList, const int listLength) {
    for (int i = 0; i < listLength; i++) {
        if (tokenList[i].type == PRINT) printf("PRINT");
        else if (tokenList[i].type == INT) printf("INT");
        else if (tokenList[i].type == NUMBER) printf("NUMBER(%d)", tokenList[i].literal.i_value);
        else if (tokenList[i].type == PLUS) printf("PLUS");
        else if (tokenList[i].type == MINUS) printf("MINUS");
        else if (tokenList[i].type == SEMICOLON) printf("SEMICOLON");
        else if (tokenList[i].type == STAR) printf("STAR");
        else if (tokenList[i].type == EF) printf("EOF");
        else if (tokenList[i].type == SLASH) printf("SLASH");
        else if (tokenList[i].type == LEFT_PAR) printf("LEFT PARENTHESIS");
        else if (tokenList[i].type == RIGHT_PAR) printf("RIGHT PARENTHESIS");
        else if (tokenList[i].type == EQUAL) printf("EQUAL");
        else if (tokenList[i].type == WORD) printf("WORD");
        else if (tokenList[i].type == CHAR) printf("CHAR");
        else if (tokenList[i].type == CHAR_LIT) printf("CHAR LIT(%c)", tokenList[i].literal.c_value);
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
    if (peek(p)->type == LEFT_PAR) {
        consume(p);
        Expression *expr = addition(p);
        if (peek(p)->type == RIGHT_PAR) consume(p);
        else perror("Expected '(' after expression");
        return expr;
    }
    if (peek(p)->type == WORD) {
        Expression *expr = malloc(sizeof(struct Expression));
        expr->type = VAR_EXPR;
        expr->as.variable.name = consume(p);
        return expr;
    }
    if (peek(p)->type == CHAR_LIT) {
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
    if (peek(p)->type == INT) {
        consume(p);
        if (peek(p)->type == WORD) {
            stmt->type = STMT_CREATE_INT;
            stmt->as.Assignment.name = consume(p);
            if (peek(p)->type == EQUAL) {
                consume(p);
                stmt->as.Assignment.value = addition(p);
            }
            else perror("Initalizing variable requires a value");
        }
        else perror("Variable requires an expression");
    }
    else if (peek(p)->type == CHAR) {
        consume(p);
        if (peek(p)->type == WORD) {
            stmt->type = STMT_CREATE_CHAR;
            stmt->as.Assignment.name = consume(p);
            if (peek(p)->type == EQUAL) {
                consume(p);
                if (peek(p)->type == CHAR_LIT) {
                    Expression *expr = malloc(sizeof(struct Expression));
                    expr->type = LITERAL;
                    expr->as.literal.value = consume(p);
                    stmt->as.Assignment.value = expr;
                }
                else perror("Initializing char variable requires a char literal");
            }
            else perror("Initializing variable requires a value");
        }
        else perror("Variable requires an expression");
    }
    else if (peek(p)->type == WORD && !is_at_end(p) && p->tokens[p->current + 1].type == EQUAL) {
        stmt->type = STMT_ASSIGN;
        stmt->as.Assignment.name = consume(p);
        consume(p);
        stmt->as.Assignment.value = addition(p);
    }
    else if (peek(p)->type == PRINT) {
        consume(p);
        expr = addition(p);
        stmt->type = STMT_PRINT;
        stmt->as.expression = expr;
    }
    else {
        expr = addition(p);
        stmt->type = STMT_EXPRESSION;
        stmt->as.expression = expr;
    }
    if (peek(p)->type == SEMICOLON) consume(p);
    else if (peek(p)->type != EF) {
        perror("; expected after an expression");
        consume(p);
    }
    return stmt;
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
