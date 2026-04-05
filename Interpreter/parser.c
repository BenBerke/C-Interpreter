#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Headers/Parser.h"
#include "Headers/Token.h"
#include "Headers/Ast.h"
#include "Headers/Interpreter.h"

Expression* comparison(Parser *p);
Expression* addition(Parser *p);
Expression* multiplication(Parser *p);
Expression* call(Parser *p);
Expression* primary(Parser *p);

Statement* parse(Parser *p);
Statement* parse_function(Parser *p);
Statement* parse_return(Parser *p);
Statement* parse_if(Parser *p);
Statement** parse_block(Parser *p, int *out_count);

struct Parser {
    const Token *tokens;
    int count;
    int current;
};

static const Token *peek(Parser *p)     { return &p->tokens[p->current]; }
static const Token *previous(Parser *p) { return &p->tokens[p->current - 1]; }
static int is_at_end(Parser *p)         { return p->current >= p->count || p->tokens[p->current].type == EF; }
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
        else if (tokenList[i].type == LEFT_PAR) printf("LEFT_PARENTHESIS");
        else if (tokenList[i].type == RIGHT_PAR) printf("RIGHT_PARENTHESIS");
        else if (tokenList[i].type == LEFT_BRACE) printf("LEFT_BRACE");
        else if (tokenList[i].type == RIGHT_BRACE) printf("RIGHT_BRACE");
        else if (tokenList[i].type == EQUAL) printf("EQUAL");
        else if (tokenList[i].type == EQUAL_EQUAL) printf("EQUAL_EQUAL");
        else if (tokenList[i].type == WORD) printf("WORD");
        else if (tokenList[i].type == CHAR) printf("CHAR");
        else if (tokenList[i].type == CHAR_LIT) printf("CHAR_LIT(%c)", tokenList[i].literal.c_value);
        else if (tokenList[i].type == FUNCTION) printf("FUNCTION");
        else if (tokenList[i].type == RETURN) printf("RETURN");
        else if (tokenList[i].type == IF) printf("IF");
        else if (tokenList[i].type == COMMA) printf("COMMA");
        else if (tokenList[i].type == LESS) printf("LESS");
        else if (tokenList[i].type == GREATER) printf("GREATER");
        else if (tokenList[i].type == LESS_EQUAL) printf("LESS_EQUAL");
        else if (tokenList[i].type == GREATER_EQUAL) printf("GREATER_EQUAL");
        else if (tokenList[i].type == GREATER_EQUAL) printf("EQUAL_EQUAL");
        else if (tokenList[i].type == BOOL) printf("BOOL");
        else if (tokenList[i].type == TRUE) printf("TRUE");
        else if (tokenList[i].type == FALSE) printf("FALSE");
        printf("\n");
    }
}

/* =========================
   Expression parsing
   ========================= */

Expression* primary(Parser *p) {
    if (peek(p)->type == NUMBER ||
        peek(p)->type == CHAR_LIT ||
        peek(p)->type == TRUE ||
        peek(p)->type == FALSE) {
        Expression *expr = malloc(sizeof(struct Expression));
        expr->type = LITERAL;
        expr->as.literal.value = consume(p);
        return expr;
        }

    if (peek(p)->type == WORD) {
        Expression *expr = malloc(sizeof(struct Expression));
        expr->type = VAR_EXPR;
        expr->as.variable.name = consume(p);
        return expr;
    }

    if (peek(p)->type == LEFT_PAR) {
        consume(p);
        Expression *expr = comparison(p);
        if (peek(p)->type == RIGHT_PAR) consume(p);
        else fprintf(stderr, "Expected ')' after expression\n");
        return expr;
    }

    fprintf(stderr, "Expected expression\n");
    return NULL;
}

Expression* call(Parser *p) {
    Expression* expr = primary(p);

    while (expr != NULL && peek(p)->type == LEFT_PAR) {
        consume(p); /* consume '(' */

        Expression** args = NULL;
        int arg_count = 0;

        if (peek(p)->type != RIGHT_PAR) {
            while (1) {
                Expression* arg = comparison(p);
                arg_count++;
                args = realloc(args, arg_count * sizeof(Expression*));
                args[arg_count - 1] = arg;

                if (peek(p)->type == COMMA) {
                    consume(p);
                    continue;
                }
                break;
            }
        }

        if (peek(p)->type == RIGHT_PAR) consume(p);
        else perror("Expected ')' after arguments");

        if (expr->type != VAR_EXPR) {
            perror("Can only call a function name");
            return expr;
        }

        Expression* new_expr = malloc(sizeof(struct Expression));
        new_expr->type = CALL_EXPR;
        new_expr->as.call.callee = expr->as.variable.name;
        new_expr->as.call.args = args;
        new_expr->as.call.arg_count = arg_count;

        free(expr);
        expr = new_expr;
    }

    return expr;
}

Expression* multiplication(Parser *p) {
    Expression *expr = call(p);

    while (peek(p)->type == STAR || peek(p)->type == SLASH) {
        const Token* op = consume(p);
        Expression *right = call(p);

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

Expression* comparison(Parser *p) {
    Expression* expr = addition(p);

    while (peek(p)->type == LESS ||
           peek(p)->type == LESS_EQUAL ||
           peek(p)->type == GREATER ||
           peek(p)->type == GREATER_EQUAL ||
           peek(p)->type == EQUAL_EQUAL) {
        const Token* op = consume(p);
        Expression* right = addition(p);

        Expression* new_node = malloc(sizeof(struct Expression));
        new_node->type = BINARY;
        new_node->as.binary.left = expr;
        new_node->as.binary.op = op;
        new_node->as.binary.right = right;
        expr = new_node;
    }

    return expr;
}


Statement** parse_block(Parser *p, int *out_count) {
    Statement** statements = NULL;
    *out_count = 0;

    while (!is_at_end(p) && peek(p)->type != RIGHT_BRACE) {
        Statement* stmt = parse(p);
        (*out_count)++;
        statements = realloc(statements, (*out_count) * sizeof(Statement*));
        statements[*out_count - 1] = stmt;
    }

    if (peek(p)->type == RIGHT_BRACE) consume(p);
    else perror("Expected '}' after block");

    return statements;
}

Statement* parse_function(Parser *p) {
    Statement* stmt = malloc(sizeof(struct Statement));
    stmt->type = STMT_FUNCTION;

    if (peek(p)->type != WORD) {
        perror("Expected function name");
        return stmt;
    }
    stmt->as.function.name = consume(p);

    if (peek(p)->type != LEFT_PAR) {
        perror("Expected '(' after function name");
        return stmt;
    }
    consume(p);

    stmt->as.function.params = NULL;
    stmt->as.function.param_count = 0;

    if (peek(p)->type != RIGHT_PAR) {
        while (1) {
            if (peek(p)->type != WORD) {
                perror("Expected parameter name");
                break;
            }

            stmt->as.function.param_count++;
            stmt->as.function.params = realloc(
                stmt->as.function.params,
                stmt->as.function.param_count * sizeof(Token*)
            );
            stmt->as.function.params[stmt->as.function.param_count - 1] = consume(p);

            if (peek(p)->type == COMMA) {
                consume(p);
                continue;
            }
            break;
        }
    }

    if (peek(p)->type == RIGHT_PAR) consume(p);
    else perror("Expected ')' after parameters");

    if (peek(p)->type != LEFT_BRACE) {
        perror("Expected '{' before function body");
        return stmt;
    }
    consume(p);

    stmt->as.function.body = parse_block(p, &stmt->as.function.body_count);
    return stmt;
}

Statement* parse_return(Parser *p) {
    Statement* stmt = malloc(sizeof(struct Statement));
    stmt->type = STMT_RETURN;
    stmt->as.return_stmt.value = NULL;

    if (peek(p)->type != SEMICOLON) {
        stmt->as.return_stmt.value = comparison(p);
    }

    if (peek(p)->type == SEMICOLON) consume(p);
    else perror("Expected ';' after return");

    return stmt;
}

Statement* parse_if(Parser *p) {
    Statement* stmt = malloc(sizeof(struct Statement));
    stmt->type = STMT_IF;

    if (peek(p)->type != LEFT_PAR) {
        perror("Expected '(' after if");
        return stmt;
    }
    consume(p);

    stmt->as.if_stmt.condition = comparison(p);

    if (peek(p)->type == RIGHT_PAR) consume(p);
    else perror("Expected ')' after if condition");

    stmt->as.if_stmt.then_branch = parse(p);
    return stmt;
}

/* =========================
   Main statement parser
   ========================= */

Statement* parse(Parser *p) {
    Statement* stmt = NULL;

    if (peek(p)->type == INT) {
        stmt = malloc(sizeof(struct Statement));
        consume(p);

        if (peek(p)->type != WORD) perror("Variable requires a name");
        stmt->type = STMT_CREATE_INT;
        stmt->as.Assignment.name = consume(p);

        if (peek(p)->type == EQUAL) consume(p);
        else perror("Initializing variable requires a value");

        stmt->as.Assignment.value = comparison(p);

        if (peek(p)->type == SEMICOLON) consume(p);
        else perror("Expected ';' after variable declaration");
        return stmt;
    }

    if (peek(p)->type == CHAR) {
        stmt = malloc(sizeof(struct Statement));
        consume(p);

        if (peek(p)->type != WORD) perror("Variable requires a name");
        stmt->type = STMT_CREATE_CHAR;
        stmt->as.Assignment.name = consume(p);

        if (peek(p)->type == EQUAL) consume(p);
        else perror("Initializing variable requires a value");

        if (peek(p)->type == CHAR_LIT) {
            Expression *expr = malloc(sizeof(struct Expression));
            expr->type = LITERAL;
            expr->as.literal.value = consume(p);
            stmt->as.Assignment.value = expr;
        } else {
            perror("Initializing char variable requires a char literal");
            stmt->as.Assignment.value = NULL;
        }

        if (peek(p)->type == SEMICOLON) consume(p);
        else perror("Expected ';' after variable declaration");
        return stmt;
    }

    if (peek(p)->type == BOOL) {
        stmt = malloc(sizeof(struct Statement));
        consume(p);

        if (peek(p)->type != WORD) perror("Variable requires a name");
        stmt->type = STMT_CREATE_BOOL;
        stmt->as.Assignment.name = consume(p);

        if (peek(p)->type == EQUAL) consume(p);
        else perror("Initializing variable requires a value");

        if (peek(p)->type == TRUE || peek(p)->type == FALSE) {
            Expression *expr = malloc(sizeof(struct Expression));
            expr->type = LITERAL;
            expr->as.literal.value = consume(p);
            stmt->as.Assignment.value = expr;
        } else {
            perror("Initializing bool variable requires true or false");
            stmt->as.Assignment.value = NULL;
        }

        if (peek(p)->type == SEMICOLON) consume(p);
        else perror("Expected ';' after variable declaration");
        return stmt;
    }

    if (peek(p)->type == WORD &&
        p->current + 1 < p->count &&
        p->tokens[p->current + 1].type == EQUAL) {
        stmt = malloc(sizeof(struct Statement));
        stmt->type = STMT_ASSIGN;
        stmt->as.Assignment.name = consume(p);
        consume(p);
        stmt->as.Assignment.value = comparison(p);

        if (peek(p)->type == SEMICOLON) consume(p);
        else perror("Expected ';' after assignment");
        return stmt;
    }

    if (peek(p)->type == PRINT) {
        stmt = malloc(sizeof(struct Statement));
        consume(p);
        stmt->type = STMT_PRINT;
        stmt->as.expression = comparison(p);

        if (peek(p)->type == SEMICOLON) consume(p);
        else perror("Expected ';' after print");
        return stmt;
    }

    if (peek(p)->type == LEFT_BRACE) {
        stmt = malloc(sizeof(struct Statement));
        consume(p);
        stmt->type = STMT_BLOCK;
        stmt->as.block.statements = parse_block(p, &stmt->as.block.count);
        return stmt;
    }

    if (peek(p)->type == FUNCTION) {
        consume(p);
        return parse_function(p);
    }

    if (peek(p)->type == RETURN) {
        consume(p);
        return parse_return(p);
    }

    if (peek(p)->type == IF) {
        consume(p);
        return parse_if(p);
    }

    stmt = malloc(sizeof(struct Statement));
    stmt->type = STMT_EXPRESSION;
    stmt->as.expression = comparison(p);

    if (peek(p)->type == SEMICOLON) consume(p);
    else perror("Expected ';' after expression");

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

    reset_runtime();

    for (int i = 0; i < program.count; i++) {
        evaluate_statement(program.statements[i]);
    }

    for (int i = 0; i < program.count; i++) {
        free_statement(program.statements[i]);
    }

    free(program.statements);
}