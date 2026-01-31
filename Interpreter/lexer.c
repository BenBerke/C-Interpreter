#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "Headers/Tokens.h"

int main(void) {
    Token *tokenList = malloc(sizeof(Token));
    Token *temp_tokenList = tokenList;
    int listSize = 0;

    char ch;
    FILE *fptr = fopen("source.txt", "r");
    if (fptr == NULL) perror("Error opening the file\n");

    char *string = malloc(1);
    char *temp_string = string;
    int count = 0;
    while ((ch = fgetc(fptr)) != EOF) {
        count++;
        string = realloc(temp_string, count);
        temp_string = string;
        *(string + count - 1) = ch;
    }

    for (int i = 0; i < count; i++) {
        tokenList = realloc(temp_tokenList, sizeof(Token));
        temp_tokenList = tokenList;
        Token newToken;
        char isValid = 0;
        char c = *(string + i);
        if (97 <= tolower(c) && tolower(c) <= 122) {
            int current_char = 0;
            char *word = malloc(1);
            char *temp_word = word;
            while (i + current_char < count && !isspace(*(string + i + current_char))) {
                *(word + current_char) = *(string + i + current_char);
                current_char++;
                word = realloc(temp_word, current_char);
                temp_word = word;
            }
            *(word + current_char) = '\0';
            if (strcmp(word, "print") == 0) {
                isValid = 1;
                newToken.type = PRINT;
            }
            else if (strcmp(word, "variable") == 0) {
                isValid = 1;
                newToken.type = VAR;
            }
            i += current_char;
            free(word);
        }
        else if (48 <= c && c <= 57) {
            int current_char = 0;
            char *word = malloc(1);
            char *temp_word = word;
            while (i + current_char < count && !isspace(*(string + i + current_char))) {
                *(word + current_char) = *(string + i + current_char);
                current_char++;
                word = realloc(temp_word, current_char);
                temp_word = word;
            }
            isValid = 1;
            i += current_char;
            *(word + current_char) = '\0';
            newToken.type = NUMBER;
            newToken.literal.i_value = atoi(word);
            free(word);
        }
        else {
            switch(c) {
                case '=': newToken.type = EQL; isValid = 1; break;
                case '+': newToken.type = PLUS; isValid = 1; break;
                case '-': newToken.type = MINUS; isValid = 1; break;
            }
        }
        listSize++;
        if (isValid == 1) {
            *(tokenList + listSize) = newToken;
        }
    }

    free(tokenList);
    free(string);
    rewind(fptr);
    fclose(fptr);
    return 0;
}