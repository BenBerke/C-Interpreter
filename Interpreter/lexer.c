#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "Headers/Token.h"
#include "Headers/Parser.h"

int main(void) {
    int rc = 0;
    Token *tokenList = NULL;
    int listLength = 0;

    int ch;
    FILE *fptr = fopen("source.txt", "r");
    if (fptr == NULL) {
        perror("Error opening the file\n");
        rc = 1;
        goto cleanup;
    }

    char *string = NULL;
    int len = 0;
    while ((ch = fgetc(fptr)) != EOF) {
        char* p = realloc(string, len + 2);
        if (!p) { rc = 1; goto cleanup; }
        string = p;
        string[len++] = (char)ch;
    }

    if (string) {
        string[len] = '\0';
        for (int i = 0; i < len; i++) {
            if (isspace((unsigned char)string[i])) continue;
            if (isalpha((unsigned char)string[i])) {
                char *current_word = NULL;
                int word_len = 0;
                while (i < len && !isspace((unsigned char)string[i]) && string[i] != '\0' && isalpha((unsigned char)string[i])) {
                    char *p = realloc(current_word, word_len+2);
                    if (!p) { free(current_word); rc = 1; goto cleanup; }
                    current_word = p;
                    current_word[word_len++] = string[i++];
                    current_word[word_len] = '\0';;
                }
                if (!current_word) goto end;
                if (!strcmp(current_word, "print")) {
                    Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                    if (!p) { free(current_word); rc = 1; goto cleanup; }
                    tokenList = p;
                    tokenList[listLength].name = "print";
                    tokenList[listLength].type = PRINT;
                    tokenList[listLength].literal.s_value = NULL;
                    listLength++;
                }
                else if (!strcmp(current_word, "int")) {
                    Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                    if (!p) { free(current_word); rc = 1; goto cleanup; }
                    tokenList = p;
                    tokenList[listLength].name = "int";
                    tokenList[listLength].type = INT;
                    tokenList[listLength].literal.s_value = NULL;
                    listLength++;
                }
                else if (!strcmp(current_word, "char")) {
                    Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                    if (!p) { free(current_word); rc = 1; goto cleanup; }
                    tokenList = p;
                    tokenList[listLength].name = "char";
                    tokenList[listLength].type = CHAR;
                    tokenList[listLength].literal.s_value = NULL;
                    listLength++;
                }
                else {
                    Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                    if (!p) { free(current_word); rc = 1; goto cleanup; }
                    tokenList = p;
                    tokenList[listLength].name = current_word;
                    tokenList[listLength].type = WORD;
                    char *copy = strdup(current_word);
                    tokenList[listLength].type = WORD;
                    tokenList[listLength].name = copy;
                    tokenList[listLength].literal.s_value = copy;
                    listLength++;
                }
                end:
                i--;
                free(current_word);
            }
            else if (isdigit((unsigned char)string[i])) {
                char *current_word = NULL;
                int word_len = 0;
                while (i < len && !isspace((unsigned char)string[i]) && string[i] != '\0' && isdigit((unsigned char)string[i])) {
                    char *p = realloc(current_word, word_len+2);
                    if (!p) { free(current_word); rc = 1; goto cleanup; }
                    current_word = p;
                    current_word[word_len++] = string[i++];
                    current_word[word_len] = '\0';;
                }
                if (!current_word) { rc = 1; goto cleanup; }
                Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                if (!p) { free(current_word); rc = 1; goto cleanup; }
                tokenList = p;
                tokenList[listLength].type = NUMBER;
                tokenList[listLength].literal.i_value = atoi(current_word);
                listLength++;
                i--;
                free(current_word);
            }
            else if (string[i] == '\'') {
                if (i + 2 >= len || string[i + 2] != '\'') { rc = 1; goto cleanup; }
                Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                if (!p) { rc = 1; goto cleanup; }
                tokenList = p;
                tokenList[listLength].type = CHAR_LIT;
                tokenList[listLength].literal.c_value = string[i + 1];
                listLength++;
                i += 2;
            }

            else if (strchr("+-;*/()=", (unsigned char)string[i])) {
                Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
                if (!p) { rc = 1; goto cleanup; }
                tokenList = p;
                switch (string[i]) {
                    case '+': tokenList[listLength].type = PLUS; break;
                    case '-': tokenList[listLength].type = MINUS; break;
                    case ';': tokenList[listLength].type = SEMICOLON; break;
                    case '*': tokenList[listLength].type = STAR; break;
                    case '/': tokenList[listLength].type = SLASH; break;
                    case '(': tokenList[listLength].type = LEFT_PAR; break;
                    case ')': tokenList[listLength].type = RIGHT_PAR; break;
                    case '=': tokenList[listLength].type = EQUAL; break;
                    default: break;
                }
                tokenList[listLength].literal.s_value = NULL;
                listLength++;
            }
        }
        Token *p = realloc(tokenList, (listLength + 1) * sizeof *tokenList);
        if (p) {
            tokenList = p;
            tokenList[listLength].type = EF;
            tokenList[listLength].literal.s_value = NULL;
            listLength++;
        }
    }

    init_parser(tokenList, listLength);
    //print_list(tokenList, listLength);

    cleanup:
    if (tokenList)
        for (int i = 0; i < listLength; i++) {
            if (tokenList[i].type == INT || tokenList[i].type == PRINT || tokenList[i].type == PLUS || tokenList[i].type == MINUS
                || tokenList[i].type == SEMICOLON || tokenList[i].type == STAR || tokenList[i].type == SLASH ||
                tokenList[i].type == LEFT_PAR || tokenList[i].type == RIGHT_PAR || tokenList[i].type == EQUAL || tokenList[i].type == CHAR)
                free(tokenList[i].literal.s_value);
            if (tokenList[i].type == WORD) free(tokenList[i].name);
        }

    free(tokenList);
    if (fptr) fclose(fptr);
    free(string);
    return rc;
}
