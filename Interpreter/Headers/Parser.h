#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Token.h"
#include "Ast.h"

void print_list(const Token* tokenList, int listLength);
void init_parser(const Token* tokenList, int listLength);

#endif
