#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Headers/Token.h"
#include "Headers/Ast.h"
#include "Headers/Interpreter.h"

#define MAX_VARS 100
#define MAX_SCOPES 100
#define MAX_FUNCTIONS 100

typedef struct {
    char keys[MAX_VARS][256];
    int values[MAX_VARS];
    int count;
} Scope;

typedef struct {
    char name[256];
    const Token** params;
    int param_count;
    Statement** body;
    int body_count;
} FunctionDef;

typedef struct {
    int value;
    int has_return;
} EvalResult;

static Scope scopes[MAX_SCOPES];
static int scope_count = 0;

static FunctionDef functions[MAX_FUNCTIONS];
static int function_count = 0;


/* =========================
   Runtime / scope helpers
   ========================= */

void reset_runtime(void) {
    scope_count = 1;
    scopes[0].count = 0;
    function_count = 0;
}

static void push_scope(void) {
    if (scope_count >= MAX_SCOPES) {
        fprintf(stderr, "Too many nested scopes\n");
        return;
    }

    scopes[scope_count].count = 0;
    scope_count++;
}

static void pop_scope(void) {
    if (scope_count > 1) {
        scope_count--;
    }
}

static int get_index_in_scope(int scope_index, const char* key) {
    for (int i = 0; i < scopes[scope_index].count; i++) {
        if (strcmp(scopes[scope_index].keys[i], key) == 0) {
            return i;
        }
    }
    return -1;
}

static void define_int(const char* name, int value) {
    Scope* scope = &scopes[scope_count - 1];
    int index = get_index_in_scope(scope_count - 1, name);

    if (index == -1) {
        if (scope->count >= MAX_VARS) {
            fprintf(stderr, "Too many variables in scope\n");
            return;
        }

        strcpy(scope->keys[scope->count], name);
        scope->values[scope->count] = value;
        scope->count++;
    } else {
        scope->values[index] = value;
    }
}

static void assign_int(const char* name, int value) {
    for (int i = scope_count - 1; i >= 0; i--) {
        int index = get_index_in_scope(i, name);
        if (index != -1) {
            scopes[i].values[index] = value;
            return;
        }
    }

    fprintf(stderr, "Undefined variable '%s'\n", name);
}

static int lookup_int(const char* name) {
    for (int i = scope_count - 1; i >= 0; i--) {
        int index = get_index_in_scope(i, name);
        if (index != -1) {
            return scopes[i].values[index];
        }
    }

    fprintf(stderr, "Undefined variable '%s'\n", name);
    return 0;
}


static FunctionDef* find_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return &functions[i];
        }
    }
    return NULL;
}

static void define_function(const Statement* stmt) {
    FunctionDef* fn = find_function(stmt->as.function.name->name);

    if (fn == NULL) {
        if (function_count >= MAX_FUNCTIONS) {
            fprintf(stderr, "Too many functions defined\n");
            return;
        }
        fn = &functions[function_count++];
    }

    strcpy(fn->name, stmt->as.function.name->name);
    fn->params = stmt->as.function.params;
    fn->param_count = stmt->as.function.param_count;
    fn->body = stmt->as.function.body;
    fn->body_count = stmt->as.function.body_count;
}


/* =========================
   Forward declarations
   ========================= */

static int eval_expr(Expression *expr);
static EvalResult execute_statement_internal(const Statement *stmt);
static EvalResult execute_statements(Statement** statements, int count);


/* =========================
   Expression evaluation
   ========================= */

static int eval_expr(Expression *expr) {
    if (expr == NULL) return 0;

    if (expr->type == LITERAL) {
        if (expr->as.literal.value->type == NUMBER) {
            return expr->as.literal.value->literal.i_value;
        }
        if (expr->as.literal.value->type == CHAR_LIT) {
            return expr->as.literal.value->literal.c_value;
        }
        return 0;
    }

    if (expr->type == VAR_EXPR) {
        return lookup_int(expr->as.variable.name->name);
    }

    if (expr->type == CALL_EXPR) {
        FunctionDef* fn = find_function(expr->as.call.callee->name);
        if (fn == NULL) {
            fprintf(stderr, "Undefined function '%s'\n", expr->as.call.callee->name);
            return 0;
        }

        if (fn->param_count != expr->as.call.arg_count) {
            fprintf(
                stderr,
                "Function '%s' expected %d arguments but got %d\n",
                fn->name,
                fn->param_count,
                expr->as.call.arg_count
            );
            return 0;
        }

        int arg_values[MAX_VARS];

        for (int i = 0; i < expr->as.call.arg_count; i++) {
            arg_values[i] = eval_expr(expr->as.call.args[i]);
        }

        push_scope();

        for (int i = 0; i < fn->param_count; i++) {
            define_int(fn->params[i]->name, arg_values[i]);
        }

        EvalResult result = execute_statements(fn->body, fn->body_count);

        pop_scope();
        return result.value;
    }

    if (expr->type == BINARY) {
        int left = eval_expr(expr->as.binary.left);
        int right = eval_expr(expr->as.binary.right);

        switch (expr->as.binary.op->type) {
            case PLUS: return left + right;
            case MINUS: return left - right;
            case STAR: return left * right;
            case SLASH: return right != 0 ? left / right : 0;

            case LESS: return left < right;
            case LESS_EQUAL: return left <= right;
            case GREATER: return left > right;
            case GREATER_EQUAL: return left >= right;
            case EQUAL_EQUAL: return left == right;

            default: return 0;
        }
    }

    return 0;
}


/* =========================
   Statement execution
   ========================= */

static EvalResult execute_statements(Statement** statements, int count) {
    EvalResult result;
    result.value = 0;
    result.has_return = 0;

    for (int i = 0; i < count; i++) {
        result = execute_statement_internal(statements[i]);
        if (result.has_return) {
            return result;
        }
    }

    return result;
}

static EvalResult execute_statement_internal(const Statement *stmt) {
    EvalResult result;
    result.value = 0;
    result.has_return = 0;

    if (stmt == NULL) return result;

    switch (stmt->type) {
        case STMT_EXPRESSION:
            result.value = eval_expr(stmt->as.expression);
            return result;

        case STMT_PRINT:
            printf("%d\n", eval_expr(stmt->as.expression));
            return result;

        case STMT_CREATE_INT:
            define_int(
                stmt->as.Assignment.name->name,
                eval_expr(stmt->as.Assignment.value)
            );
            return result;

        case STMT_CREATE_CHAR:
            if (stmt->as.Assignment.value != NULL &&
                stmt->as.Assignment.value->type == LITERAL &&
                stmt->as.Assignment.value->as.literal.value->type == CHAR_LIT) {
                define_int(
                    stmt->as.Assignment.name->name,
                    stmt->as.Assignment.value->as.literal.value->literal.c_value
                );
            }
            return result;

        case STMT_ASSIGN:
            assign_int(
                stmt->as.Assignment.name->name,
                eval_expr(stmt->as.Assignment.value)
            );
            return result;

        case STMT_BLOCK:
            push_scope();
            result = execute_statements(stmt->as.block.statements, stmt->as.block.count);
            pop_scope();
            return result;

        case STMT_FUNCTION:
            define_function(stmt);
            return result;

        case STMT_RETURN:
            result.value = eval_expr(stmt->as.return_stmt.value);
            result.has_return = 1;
            return result;

        case STMT_IF:
            if (eval_expr(stmt->as.if_stmt.condition)) {
                return execute_statement_internal(stmt->as.if_stmt.then_branch);
            }
            return result;

        default:
            fprintf(stderr, "Unknown statement type\n");
            return result;
    }
}


/* =========================
   Public wrappers
   ========================= */

int evaluate_expression(Expression *expr) {
    return eval_expr(expr);
}

int evaluate_statement(const Statement *stmt) {
    return execute_statement_internal(stmt).value;
}


/* =========================
   Memory cleanup
   ========================= */

void free_expression(Expression *expr) {
    if (expr == NULL) return;

    switch (expr->type) {
        case BINARY:
            free_expression(expr->as.binary.left);
            free_expression(expr->as.binary.right);
            break;

        case CALL_EXPR:
            for (int i = 0; i < expr->as.call.arg_count; i++) {
                free_expression(expr->as.call.args[i]);
            }
            free(expr->as.call.args);
            break;

        case LITERAL:
        case VAR_EXPR:
        default:
            break;
    }

    free(expr);
}

void free_statement(Statement *stmt) {
    if (stmt == NULL) return;

    switch (stmt->type) {
        case STMT_EXPRESSION:
        case STMT_PRINT:
            free_expression(stmt->as.expression);
            break;

        case STMT_CREATE_INT:
        case STMT_CREATE_CHAR:
        case STMT_ASSIGN:
            free_expression(stmt->as.Assignment.value);
            break;

        case STMT_BLOCK:
            for (int i = 0; i < stmt->as.block.count; i++) {
                free_statement(stmt->as.block.statements[i]);
            }
            free(stmt->as.block.statements);
            break;

        case STMT_FUNCTION:
            for (int i = 0; i < stmt->as.function.body_count; i++) {
                free_statement(stmt->as.function.body[i]);
            }
            free(stmt->as.function.body);
            free(stmt->as.function.params);
            break;

        case STMT_RETURN:
            free_expression(stmt->as.return_stmt.value);
            break;

        case STMT_IF:
            free_expression(stmt->as.if_stmt.condition);
            free_statement(stmt->as.if_stmt.then_branch);
            break;

        default:
            break;
    }

    free(stmt);
}