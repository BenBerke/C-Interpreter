#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Ast.h"
#include "Dictionary.h"

extern Dictionary variables;
extern int size;
extern char keys[100][256];
extern int values[100];

int evaluate_expression(Expression *expr);
int evaluate_statement(const Statement *stmt);
void free_expression(Expression *expr);
void free_statement(Statement *stmt);

#endif