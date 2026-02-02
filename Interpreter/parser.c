#include <stdio.h>
#include <stdlib.h>
#include "Headers/Token.h"

void print_list(const Token* tokenList, const int listLength) {
    for (int i = 0; i < listLength; i++) {
        if (tokenList[i].type == PRINT) printf("PRINT");
        else if (tokenList[i].type == VAR) printf("VAR");
        else if (tokenList[i].type == NUMBER) printf("NUMBER(%d)", tokenList[i].literal.i_value);
        else if (tokenList[i].type == PLUS) printf("PLUS");
        else if (tokenList[i].type == MINUS) printf("MINUS");
        printf("\n");
    }
}

void init_parser(const Token* tokenList, const int listLength) {

}