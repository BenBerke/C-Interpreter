#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

//#include "Headers/Tokens.h"

int main(void) {
    // File readers
    char ch;
    FILE *fptr = fopen("source.txt", "r");
    if (fptr == NULL) perror("Error opening the file\n");

    // Read the file and allocate enough space
    char *string = malloc(1);
    char *temp_string = string;
    int count = 0;
    while ((ch = fgetc(fptr)) != EOF) {
        count++;
        string = realloc(temp_string, count);
        temp_string = string;
        *(string + count - 1) = ch; // Assign the character to the newly allocated space
    }

    for (int i = 0; i < count; i++) {
        char c = *(string + i);
        if (97 <= tolower(c) && tolower(c) <= 122) {
            int current_char = 0;
            char *word = malloc(1);
            char *temp_word = word;
            while (*(string + i + current_char) != ' ' && i + current_char < count) {
                *(word + current_char) = *(string + i + current_char);
                current_char++;
                word = realloc(temp_word, current_char);
                temp_word = word;
            }
            for (int j = 0; j < current_char; j++) {
                printf("%c", *(temp_word + j));
            }
            printf("\n");
            i += current_char;
            free(word);
        }
    }

    free(string);
    rewind(fptr);
    fclose(fptr);
    return 0;
}