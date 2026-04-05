#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stddef.h>

typedef struct{
    char* key;
    double* values;
    int size;
} Dictionary;

int hash(const char* value);
void add_to_dict(Dictionary* dict, char* key, double value);

#endif
