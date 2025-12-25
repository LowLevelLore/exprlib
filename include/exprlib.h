// SPDX-License-Identifier: MIT
/*
 * ExprLib — Expression parsing and evaluation library
 * Copyright (c) 2025 LowLevelLore (https://github.com/LowLevelLore)
 */

#ifndef __EXPRLIB_H
#define __EXPRLIB_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char *string;

typedef enum {
    EXPRLIB_SUCCESS = 0,
    EXPRLIB_ERROR_SYNTAX = 1,
    EXPRLIB_ERROR_UNDEFINED_VARIABLE = 2,
    EXPRLIB_ERROR_DIVISION_BY_ZERO = 3,
    EXPRLIB_ERROR_MALLOC_FAILED = 4,
    EXPRLIB_ERROR_INVALID_ARGUMENT = 5,
    EXPRLIB_ERROR_FUNCTION_NOT_FOUND = 6,
    EXPRLIB_ERROR_NULL = 7,
    EXPRLIB_ERROR_DUPLICATE_FUNCTION = 8,
    EXPRLIB_ERROR_UNKNOWN = 9
} ExprLibError;

static const char *EXPRLIB_ERROR_MESSAGES[] = {"Success",
                                               "Syntax Error",
                                               "Undefined Variable",
                                               "Division by Zero",
                                               "Malloc Failed",
                                               "Invalid Argument",
                                               "Function Not Found",
                                               "Null Error",
                                               "Duplicate Function",
                                               "Unknown Error"};

extern ExprLibError EXPRLIB_ERROR;

typedef struct {
    string name;
    double *value;
} ExprLibVariable;

typedef struct {
    string name;
    double value;
} ExprLibConstant;

typedef struct {
    ExprLibVariable *variables;
    int var_count;
} ExprContext;

typedef enum {
    EXPR_NODE_NUMBER,
    EXPR_NODE_VARIABLE,
    EXPR_NODE_OPERATOR,
    EXPR_NODE_FUNCTION_CALL
} ExprNodeType;

typedef double (*ExprLibFnPtr)(const double *args, int argc);
typedef struct ExprNode ExprNode;

typedef struct {
    string name;
    ExprNode **args;
    int argc;
} ExprNodeFunctionCall;

typedef struct {
    char op;
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNodeOperator;

struct ExprNode {
    ExprNodeType type;
    union {
        double number;
        string variable_name;
        ExprNodeOperator op_node;
        ExprNodeFunctionCall fn_call;
    } data;
};

/* AST construction */
ExprNode *create_number_node(double value);
ExprNode *create_variable_node(const string name);
ExprNode *create_operator_node(char op, ExprNode *left, ExprNode *right);
ExprNode *create_function_node(const string name, ExprNode **args, int argc);

/* Parsing helpers */
double parse_number(const char **expr_ptr, bool *found);
bool is_defined_variable(const string name, const ExprContext *context);
ExprNode *parse_unary(const char **expr_ptr, const ExprContext *context);
ExprNode *parse_binary_rhs(int expr_prec, ExprNode *lhs, const char **expr_ptr,
                           const ExprContext *context);
ExprNode *parse_internal(const char **expr_ptr, const ExprContext *context);
double evaluate_node(const ExprNode *node, const ExprContext *context);

/* Public API */
void exprlib_free(ExprNode *node);
void print_expr_tree(const ExprNode *node, int indent);
ExprNode *exprlib_parse(const string expression, const ExprContext *context);
double exprlib_evaluate(const ExprNode *expr, const ExprContext *context);
bool exprlib_register_function(const char *name, int arity, ExprLibFnPtr fn);
void exprlib_clear_functions(void);
void exprlib_init(void);

#ifdef __cplusplus
}
#endif

#endif // __EXPRLIB_H