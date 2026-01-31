#ifndef PARSER_H
#define PARSER_H
#include "Tokens.h"

typedef struct {
    Token *tokens;
    int num_tokens;
    int current;
} Parser;

void init_parser(Parser *parser, Token *tokens, int num_tokens);
void print_tokens(Parser *parser);
Token *previous();

#endif