#include <stdio.h>
#include "Headers/Tokens.h"
#include "Headers/Parser.h"

void print_tokens(Parser *parser) {
    printf("Current Token Type: %d\n", parser->tokens[parser->current].type);
}

Token *previous() {

}

void init_parser(Parser *parser, Token *tokens, int num_tokens) {
    parser->tokens = tokens;
    parser->num_tokens = num_tokens;
    parser->current = 0;
}