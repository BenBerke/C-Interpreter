#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "Ast.h"

int evaluate_expression(Expression *expr);
int evaluate_statement(const Statement *stmt);

void free_expression(Expression *expr);
void free_statement(Statement *stmt);

#endif
