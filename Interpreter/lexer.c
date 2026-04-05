#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "Headers/Token.h"
#include "Headers/Parser.h"

static int add_token(Token **tokenList, int *listLength, Token token) {
    Token *p = realloc(*tokenList, (*listLength + 1) * sizeof(Token));
    if (!p) return 0;
    *tokenList = p;
    (*tokenList)[*listLength] = token;
    (*listLength)++;
    return 1;
}

int main(void) {
    int rc = 0;
    Token *tokenList = NULL;
    int listLength = 0;

    int ch;
    FILE *fptr = fopen("source.txt", "r");
    if (fptr == NULL) {
        perror("Error opening file");
        return 1;
    }

    char *string = NULL;
    int len = 0;

    while ((ch = fgetc(fptr)) != EOF) {
        char *p = realloc(string, len + 2);
        if (!p) {
            rc = 1;
            goto cleanup;
        }
        string = p;
        string[len++] = (char)ch;
    }

    if (string == NULL) {
        string = malloc(1);
        if (!string) {
            rc = 1;
            goto cleanup;
        }
        string[0] = '\0';
    } else {
        string[len] = '\0';
    }

    for (int i = 0; i < len; i++) {
        if (isspace((unsigned char)string[i])) continue;

        /* identifiers / keywords */
        if (isalpha((unsigned char)string[i]) || string[i] == '_') {
            int start = i;
            while (i < len &&
                   (isalnum((unsigned char)string[i]) || string[i] == '_')) {
                i++;
            }

            int word_len = i - start;
            char *current_word = malloc(word_len + 1);
            if (!current_word) {
                rc = 1;
                goto cleanup;
            }

            memcpy(current_word, &string[start], word_len);
            current_word[word_len] = '\0';

            Token token;
            token.name = NULL;
            token.literal.s_value = NULL;

            if (!strcmp(current_word, "print")) {
                token.type = PRINT;
                token.name = "print";
                free(current_word);
            }
            else if (!strcmp(current_word, "int")) {
                token.type = INT;
                token.name = "int";
                free(current_word);
            }
            else if (!strcmp(current_word, "char")) {
                token.type = CHAR;
                token.name = "char";
                free(current_word);
            }
            else if (!strcmp(current_word, "function")) {
                token.type = FUNCTION;
                token.name = "function";
                free(current_word);
            }
            else if (!strcmp(current_word, "return")) {
                token.type = RETURN;
                token.name = "return";
                free(current_word);
            }
            else if (!strcmp(current_word, "if")) {
                token.type = IF;
                token.name = "if";
                free(current_word);
            }
            else if (!strcmp(current_word, "bool")) {
                token.type = BOOL;
                token.name = "bool";
                free(current_word);
            }
            else if (!strcmp(current_word, "true")) {
                token.type = TRUE;
                token.name = "true";
                free(current_word);
            }
            else if (!strcmp(current_word, "false")) {
                token.type = FALSE;
                token.name = "false";
                free(current_word);
            }
            else {
                token.type = WORD;
                token.name = current_word;
                token.literal.s_value = current_word;
            }

            if (!add_token(&tokenList, &listLength, token)) {
                if (token.type == WORD) free(current_word);
                rc = 1;
                goto cleanup;
            }

            i--;
            continue;
        }

        /* numbers */
        if (isdigit((unsigned char)string[i])) {
            int start = i;
            while (i < len && isdigit((unsigned char)string[i])) {
                i++;
            }

            int word_len = i - start;
            char *current_word = malloc(word_len + 1);
            if (!current_word) {
                rc = 1;
                goto cleanup;
            }

            memcpy(current_word, &string[start], word_len);
            current_word[word_len] = '\0';

            Token token;
            token.type = NUMBER;
            token.name = NULL;
            token.literal.i_value = atoi(current_word);

            free(current_word);

            if (!add_token(&tokenList, &listLength, token)) {
                rc = 1;
                goto cleanup;
            }

            i--;
            continue;
        }

        /* char literals */
        if (string[i] == '\'') {
            if (i + 2 >= len || string[i + 2] != '\'') {
                fprintf(stderr, "Invalid char literal\n");
                rc = 1;
                goto cleanup;
            }

            Token token;
            token.type = CHAR_LIT;
            token.name = NULL;
            token.literal.c_value = string[i + 1];

            if (!add_token(&tokenList, &listLength, token)) {
                rc = 1;
                goto cleanup;
            }

            i += 2;
            continue;
        }

        /* two-character operators FIRST */
        if (i + 1 < len) {
            Token token;
            token.name = NULL;
            token.literal.s_value = NULL;

            if (string[i] == '<' && string[i + 1] == '=') {
                token.type = LESS_EQUAL;
                if (!add_token(&tokenList, &listLength, token)) {
                    rc = 1;
                    goto cleanup;
                }
                i++;
                continue;
            }

            if (string[i] == '>' && string[i + 1] == '=') {
                token.type = GREATER_EQUAL;
                if (!add_token(&tokenList, &listLength, token)) {
                    rc = 1;
                    goto cleanup;
                }
                i++;
                continue;
            }

            if (string[i] == '=' && string[i + 1] == '=') {
                token.type = EQUAL_EQUAL;
                if (!add_token(&tokenList, &listLength, token)) {
                    rc = 1;
                    goto cleanup;
                }
                i++;
                continue;
            }
        }

        /* single-character tokens */
        if (strchr("+-;*/()=,{}<>", (unsigned char)string[i])) {
            Token token;
            token.name = NULL;
            token.literal.s_value = NULL;

            switch (string[i]) {
                case '+': token.type = PLUS; break;
                case '-': token.type = MINUS; break;
                case ';': token.type = SEMICOLON; break;
                case '*': token.type = STAR; break;
                case '/': token.type = SLASH; break;
                case '(': token.type = LEFT_PAR; break;
                case ')': token.type = RIGHT_PAR; break;
                case '=': token.type = EQUAL; break;
                case ',': token.type = COMMA; break;
                case '{': token.type = LEFT_BRACE; break;
                case '}': token.type = RIGHT_BRACE; break;
                case '<': token.type = LESS; break;
                case '>': token.type = GREATER; break;
                default:
                    fprintf(stderr, "Unknown token: %c\n", string[i]);
                    rc = 1;
                    goto cleanup;
            }

            if (!add_token(&tokenList, &listLength, token)) {
                rc = 1;
                goto cleanup;
            }

            continue;
        }

        fprintf(stderr, "Unexpected character: %c\n", string[i]);
        rc = 1;
        goto cleanup;
    }

    {
        Token eof_token;
        eof_token.type = EF;
        eof_token.name = NULL;
        eof_token.literal.s_value = NULL;

        if (!add_token(&tokenList, &listLength, eof_token)) {
            rc = 1;
            goto cleanup;
        }
    }

    //print_list(tokenList, listLength);
    init_parser(tokenList, listLength);

cleanup:
    if (tokenList) {
        for (int i = 0; i < listLength; i++) {
            if (tokenList[i].type == WORD) {
                free(tokenList[i].name);
            }
        }
    }

    free(tokenList);
    if (fptr) fclose(fptr);
    free(string);
    return rc;
}