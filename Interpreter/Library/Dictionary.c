#include "../Headers/Dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(const char* value) {
    int i = 0;
    int sum = 0;
    while (value[i] != '\0') {
        sum += value[i];
        i++;
    }
    printf("sum: %d", sum);
    return sum;
}

void add_to_dict(Dictionary* dict, char* key, double value) {
    dict->size++;
    int position = hash(key) % dict->size;

    if (dict->size == 1) dict->values = (double*)malloc(sizeof(double));
    else dict->values = (double*)realloc(dict->values, dict->size * sizeof(double));

    printf("value: %d", value);
    dict->values[position] = value;
}