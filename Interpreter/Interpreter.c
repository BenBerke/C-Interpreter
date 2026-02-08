#include <stdio.h>
#include <stdlib.h>

#include "Headers/Token.h"
#include "Headers/Ast.h"
#include "Headers/Interpreter.h"

#include <string.h>
#define MAX_SIZE 100

int size = 0; // Current number of elements in the map
char keys[MAX_SIZE][100]; // Array to store the keys
int values[MAX_SIZE]; // Array to store the values

// Function to get the index of a key in the keys array
int getIndex(char key[])
{
    for (int i = 0; i < size; i++) {
        if (strcmp(keys[i], key) == 0) {
            return i;
        }
    }
    return -1; // Key not found
}

// Function to insert a key-value pair into the map
void create_variable(const Variable *var)
{
    if (var->type == TYPE_INT) {
        const int index = getIndex(var->name);
        if (index == -1) { // Key not found
            strcpy(keys[size], var->name);
            values[size] = var->data.i_value;
            size++;
        }
        else values[index] = var->data.i_value;
    }
}

// Function to get the value of a key in the map
int getValue(char key[])
{
    int index = getIndex(key);
    if (index == -1) { // Key not found
        return -1;
    }
    else { // Key found
        return values[index];
    }
}

// Function to print the map
void printMap()
{
    for (int i = 0; i < size; i++) {
        printf("%s: %d\n", keys[i], values[i]);
    }
}


int evaluate_expression(Expression *expr) {
    if (expr == NULL) return 0;
    if (expr->type == LITERAL) return expr->as.literal.value->literal.i_value;
    if (expr->type == BINARY) {
        const int left = evaluate_expression(expr->as.binary.left);
        const int right = evaluate_expression(expr->as.binary.right);
        switch (expr->as.binary.op->type) {
            case PLUS: return left + right;
            case MINUS: return left - right;
            case STAR:  return left * right;
            case SLASH:  return left / right;
            default: return 0;
        }
    }
    if (expr->type == VAR_EXPR) return getValue(expr->as.variable.name->name);

    return 0;
}

int evaluate_statement(const Statement *stmt) {
    if (stmt == NULL) return 0;
    if (stmt->type == STMT_EXPRESSION) return evaluate_expression(stmt->as.expression);
    if (stmt->type == STMT_PRINT) printf("%d\n", evaluate_expression(stmt->as.expression));
    else if (stmt->type == STMT_CREATE_INT) {
        Variable var;
        var.type = TYPE_INT;
        var.name = (char*)stmt->as.Assignment.name->name;
        var.data.i_value = evaluate_expression(stmt->as.Assignment.value);
        create_variable(&var);
    }
    else if (stmt->type == STMT_CREATE_CHAR) {
        char c = stmt->as.Assignment.value->as.literal.value->literal.c_value;
        Variable var;
        var.type = TYPE_CHAR;
        var.name = (char*)stmt->as.Assignment.name->name;
        var.data.c_value = &c;
        create_variable(&var);
        printMap();
    }
    else if (stmt->type == STMT_ASSIGN) values[getIndex(stmt->as.Assignment.name->name)] = evaluate_expression(stmt->as.Assignment.value);
    else perror("Unknown statement type");
    return 0;
}

void free_expression(Expression *expr) {
    if (expr == NULL) return;
    if (expr->type == BINARY) {
        free_expression(expr->as.binary.left);
        free_expression(expr->as.binary.right);
    }
    free(expr);
}

void free_statement(Statement *stmt) {
    if (!stmt) return;

    if (stmt->type == STMT_EXPRESSION || stmt->type == STMT_PRINT) {
        free_expression(stmt->as.expression);
    } else if (stmt->type == STMT_CREATE_INT || stmt->type == STMT_ASSIGN) {
        free_expression(stmt->as.Assignment.value);
    }

    free(stmt);
}
