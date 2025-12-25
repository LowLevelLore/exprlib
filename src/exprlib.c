#include "exprlib.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ExprLibError EXPRLIB_ERROR = EXPRLIB_SUCCESS;

typedef struct {
    string name;
    int arity; // -1 = variadic
    ExprLibFnPtr fn;
} ExprLibFunction;

static ExprLibFunction *g_functions = NULL;
static int g_function_count = 0;

static ExprLibConstant *g_constants = NULL;
static int g_constant_count = 0;

bool exprlib_register_constant(const char *name, double value) {
    g_constants =
        realloc(g_constants, (g_constant_count + 1) * sizeof(ExprLibConstant));
    if (!g_constants) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return false;
    }
    string name_copy = strdup(name);
    if (!name_copy) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return false;
    }
    g_constants[g_constant_count].name = name_copy;
    g_constants[g_constant_count].value = value;
    g_constant_count++;
    return true;
}

/* Trigonometric */
static double fn_sin(const double *a, int n) {
    assert(n == 1);
    return sin(a[0]);
}
static double fn_cos(const double *a, int n) {
    assert(n == 1);
    return cos(a[0]);
}
static double fn_tan(const double *a, int n) {
    assert(n == 1);
    return tan(a[0]);
}
static double fn_cot(const double *a, int n) {
    assert(n == 1);
    return 1.0 / tan(a[0]);
}
static double fn_sec(const double *a, int n) {
    assert(n == 1);
    return 1.0 / cos(a[0]);
}
static double fn_cosec(const double *a, int n) {
    assert(n == 1);
    return 1.0 / sin(a[0]);
}

/* Inverse trigonometric */
static double fn_asin(const double *a, int n) {
    assert(n == 1);
    return asin(a[0]);
}
static double fn_acos(const double *a, int n) {
    assert(n == 1);
    return acos(a[0]);
}
static double fn_atan(const double *a, int n) {
    assert(n == 1);
    return atan(a[0]);
}

/* Powers and roots */
static double fn_pow(const double *a, int n) {
    assert(n == 2);
    return pow(a[0], a[1]);
}
static double fn_sqrt(const double *a, int n) {
    assert(n == 1);
    return sqrt(a[0]);
}
static double fn_cbrt(const double *a, int n) {
    assert(n == 1);
    return cbrt(a[0]);
}

/* Logarithms */
static double fn_log(const double *a, int n) {
    assert(n == 1);
    return log(a[0]);
} /* natural log */
static double fn_log10(const double *a, int n) {
    assert(n == 1);
    return log10(a[0]);
}

/* Exponential */
static double fn_exp(const double *a, int n) {
    assert(n == 1);
    return exp(a[0]);
}

/* Absolute & sign */
static double fn_abs(const double *a, int n) {
    assert(n == 1);
    return fabs(a[0]);
}

/* Rounding */
static double fn_floor(const double *a, int n) {
    assert(n == 1);
    return floor(a[0]);
}
static double fn_ceil(const double *a, int n) {
    assert(n == 1);
    return ceil(a[0]);
}
static double fn_round(const double *a, int n) {
    assert(n == 1);
    return round(a[0]);
}

// Conversion
static double fn_deg2rad(const double *a, int n) {
    assert(n == 1);
    return a[0] * (M_PI / 180.0);
}
static double fn_rad2deg(const double *a, int n) {
    assert(n == 1);
    return a[0] * (180.0 / M_PI);
}

// Min, Max
static double fn_min(const double *a, int n) {
    assert(n >= 1);
    double min_val = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] < min_val) {
            min_val = a[i];
        }
    }
    return min_val;
}
static double fn_max(const double *a, int n) {
    assert(n >= 1);
    double max_val = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] > max_val) {
            max_val = a[i];
        }
    }
    return max_val;
}

static double factorial(const double *a, int n) {
    assert(n == 1);
    if (a[0] < 0) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_INVALID_ARGUMENT;
        return 0.0;
    }
    double result = 1.0;
    for (int i = 1; i <= (int)a[0]; i++) {
        result *= i;
    }
    return result;
}

// nCr and nPr
static double nCr(const double *a, int n) {
    assert(n == 2);
    if (a[0] < 0 || a[1] < 0 || a[1] > a[0]) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_INVALID_ARGUMENT;
        return 0.0;
    }
    return factorial((double[]){a[0]}, 1) /
           (factorial((double[]){a[1]}, 1) *
            factorial((double[]){a[0] - a[1]}, 1));
}

static double nPr(const double *a, int n) {
    assert(n == 2);
    if (a[0] < 0 || a[1] < 0 || a[1] > a[0]) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_INVALID_ARGUMENT;
        return 0.0;
    }
    return factorial((double[]){a[0]}, 1) /
           factorial((double[]){a[0] - a[1]}, 1);
}

static void exprlib_register_builtins(void) {
    /* Trigonometric */
    exprlib_register_function("sin", 1, fn_sin);
    exprlib_register_function("cos", 1, fn_cos);
    exprlib_register_function("tan", 1, fn_tan);
    exprlib_register_function("cot", 1, fn_cot);
    exprlib_register_function("sec", 1, fn_sec);
    exprlib_register_function("cosec", 1, fn_cosec);

    /* Inverse trigonometric */
    exprlib_register_function("asin", 1, fn_asin);
    exprlib_register_function("acos", 1, fn_acos);
    exprlib_register_function("atan", 1, fn_atan);

    /* Powers and roots */
    exprlib_register_function("pow", 2, fn_pow);
    exprlib_register_function("sqrt", 1, fn_sqrt);
    exprlib_register_function("cbrt", 1, fn_cbrt);

    /* Logarithms */
    exprlib_register_function("ln", 1, fn_log); /* ln */
    exprlib_register_function("log10", 1, fn_log10);

    /* Exponential */
    exprlib_register_function("exp", 1, fn_exp);

    /* Absolute & rounding */
    exprlib_register_function("abs", 1, fn_abs);
    exprlib_register_function("floor", 1, fn_floor);
    exprlib_register_function("ceil", 1, fn_ceil);
    exprlib_register_function("round", 1, fn_round);

    /* Conversion */
    exprlib_register_function("deg2rad", 1, fn_deg2rad);
    exprlib_register_function("rad2deg", 1, fn_rad2deg);

    /* Min, Max */
    exprlib_register_function("min", -1, fn_min);
    exprlib_register_function("max", -1, fn_max);

    /* Factorial */
    exprlib_register_function("factorial", 1, factorial);

    /* nCr and nPr */
    exprlib_register_function("nCr", 2, nCr);
    exprlib_register_function("nPr", 2, nPr);

    /* Constants */
    exprlib_register_constant("pi", M_PI); /* π */
    exprlib_register_constant("e", M_E);   /* Euler's number */

    /* Common derived constants */
    exprlib_register_constant("tau", 2.0 * M_PI); /* 2π */
    exprlib_register_constant("phi",
                              (1.0 + sqrt(5.0)) / 2.0); /* Golden ratio */

    /* Square roots */
    exprlib_register_constant("sqrt2", M_SQRT2);   /* √2 */
    exprlib_register_constant("sqrt3", sqrt(3.0)); /* √3 */
    exprlib_register_constant("sqrt5", sqrt(5.0)); /* √5 */

    /* Logarithmic constants */
    exprlib_register_constant("ln2", M_LN2);       /* ln(2) */
    exprlib_register_constant("ln10", M_LN10);     /* ln(10) */
    exprlib_register_constant("log2e", M_LOG2E);   /* log2(e) */
    exprlib_register_constant("log10e", M_LOG10E); /* log10(e) */

    /* Inverse pi multiples (used in physics/math derivations) */
    exprlib_register_constant("invpi", 1.0 / M_PI);          /* 1/π */
    exprlib_register_constant("inv2pi", 1.0 / (2.0 * M_PI)); /* 1/(2π) */
}

bool exprlib_register_function(const char *name, int arity, ExprLibFnPtr fn) {
    if (!name || !*name || !fn) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_NULL;
        return false;
    }

    if (arity < -1) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_INVALID_ARGUMENT;
        return false;
    }

    /* reject duplicates */
    for (int i = 0; i < g_function_count; ++i) {
        if (strcmp(g_functions[i].name, name) == 0) {
            EXPRLIB_ERROR = EXPRLIB_ERROR_DUPLICATE_FUNCTION;
            return false;
        }
    }

    /* duplicate name */
    char *name_copy = strdup(name);
    if (!name_copy) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return false;
    }

    /* grow registry */
    ExprLibFunction *new_table =
        realloc(g_functions, sizeof(ExprLibFunction) * (g_function_count + 1));
    if (!new_table) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        free(name_copy);
        return false;
    }

    g_functions = new_table;

    g_functions[g_function_count].name = name_copy;
    g_functions[g_function_count].arity = arity;
    g_functions[g_function_count].fn = fn;

    g_function_count++;

    return true;
}

const ExprLibFunction *find_function(const char *name) {
    for (int i = 0; i < g_function_count; ++i) {
        if (strcmp(g_functions[i].name, name) == 0)
            return &g_functions[i];
    }
    return NULL;
}

static int get_precedence(char op) {
    switch (op) {
    case '+':
    case '-':
        return 10;
    case '*':
    case '/':
        return 20;
    case '^':
        return 30;
    default:
        return -1;
    }
}

static bool is_right_associative(char op) { return op == '^'; }

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i)
        putchar(' ');
}

void exprlib_free(ExprNode *node) {
    if (!node)
        return;

    switch (node->type) {
    case EXPR_NODE_NUMBER:
        /* nothing extra to free */
        break;

    case EXPR_NODE_VARIABLE:
        free(node->data.variable_name);
        break;

    case EXPR_NODE_OPERATOR:
        exprlib_free(node->data.op_node.left);
        exprlib_free(node->data.op_node.right);
        break;
    case EXPR_NODE_FUNCTION_CALL:
        for (int i = 0; i < node->data.fn_call.argc; i++) {
            exprlib_free(node->data.fn_call.args[i]);
        }
        free(node->data.fn_call.args);
        free(node->data.fn_call.name);
        break;
    }

    free(node);
}

double parse_number(const char **expr_ptr, bool *found) {
    *found = false;
    while (**expr_ptr == ' ') {
        (*expr_ptr)++;
    }
    double value = 0.0;
    while (**expr_ptr >= '0' && **expr_ptr <= '9') {
        *found = true;
        value = value * 10 + (**expr_ptr - '0');
        (*expr_ptr)++;
    }
    if (**expr_ptr == '.') {
        (*expr_ptr)++;
        double frac = 0.1;
        while (**expr_ptr >= '0' && **expr_ptr <= '9') {
            value += (**expr_ptr - '0') * frac;
            frac *= 0.1;
            (*expr_ptr)++;
        }
    }
    return value;
}

bool is_defined_variable(const string name, const ExprContext *context) {
    for (int i = 0; i < g_constant_count; i++) {
        if (strcmp(g_constants[i].name, name) == 0) {
            return true;
        }
    }
    for (int i = 0; i < context->var_count; i++) {
        if (strcmp(context->variables[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

ExprNode *create_number_node(double value) {
    EXPRLIB_ERROR = EXPRLIB_SUCCESS;
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return NULL;
    }
    node->type = EXPR_NODE_NUMBER;
    node->data.number = value;
    return node;
}

ExprNode *create_variable_node(const string name) {
    EXPRLIB_ERROR = EXPRLIB_SUCCESS;
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return NULL;
    }
    node->type = EXPR_NODE_VARIABLE;
    node->data.variable_name = strdup(name);
    return node;
}

ExprNode *create_operator_node(char op, ExprNode *left, ExprNode *right) {
    EXPRLIB_ERROR = EXPRLIB_SUCCESS;
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return NULL;
    }
    node->type = EXPR_NODE_OPERATOR;
    node->data.op_node.op = op;
    node->data.op_node.left = left;
    node->data.op_node.right = right;
    return node;
}

ExprNode *create_function_node(const string name, ExprNode **args, int argc) {
    ExprNode *n = malloc(sizeof(*n));
    if (!n) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        return NULL;
    }

    char *dup = strdup(name);
    if (!dup) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
        free(n);
        return NULL;
    }

    n->type = EXPR_NODE_FUNCTION_CALL;
    n->data.fn_call.name = dup;
    n->data.fn_call.args = args; /* ownership transferred */
    n->data.fn_call.argc = argc;

    return n;
}

ExprNode *parse_unary(const char **expr_ptr, const ExprContext *context) {
    while (**expr_ptr == ' ')
        (*expr_ptr)++;

    /* unary minus */
    if (**expr_ptr == '-') {
        (*expr_ptr)++;
        ExprNode *operand = parse_unary(expr_ptr, context);
        if (!operand || EXPRLIB_ERROR != EXPRLIB_SUCCESS)
            return NULL;
        ExprNode *zero = create_number_node(0.0);
        if (!zero || EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
            exprlib_free(operand);
            return NULL;
        }
        ExprNode *node = create_operator_node('-', zero, operand);
        if (!node || EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
            exprlib_free(zero);
            exprlib_free(operand);
            return NULL;
        }
        return node;
    }

    /* parenthesized expression */
    if (**expr_ptr == '(') {
        (*expr_ptr)++;

        ExprNode *node = parse_internal(expr_ptr, context);

        if (!node || EXPRLIB_ERROR != EXPRLIB_SUCCESS)
            return NULL;

        if (**expr_ptr != ')') {
            EXPRLIB_ERROR = EXPRLIB_ERROR_SYNTAX;
            printf("SYNTAX ERROR: %s\n", *expr_ptr);
            printf("            : ^ Closing Paren Expected\n");
            exprlib_free(node);
            return NULL;
        }

        (*expr_ptr)++;
        return node;
    }

    /* number */
    bool found_number = false;
    double number = parse_number(expr_ptr, &found_number);
    if (found_number)
        return create_number_node(number);

    /* variable */
    if ((**expr_ptr >= 'a' && **expr_ptr <= 'z') ||
        (**expr_ptr >= 'A' && **expr_ptr <= 'Z')) {
        const char *start = *expr_ptr;
        while ((**expr_ptr >= 'a' && **expr_ptr <= 'z') ||
               (**expr_ptr >= 'A' && **expr_ptr <= 'Z') ||
               (**expr_ptr >= '0' && **expr_ptr <= '9') ||
               (**expr_ptr == '_')) {
            (*expr_ptr)++;
        }

        size_t len = *expr_ptr - start;
        char *name = malloc(len + 1);
        if (!name) {
            EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
            return NULL;
        }
        snprintf(name, len + 1, "%.*s", (int)len, start);

        /* skip whitespace to check for '(' (function call) */
        const char *look = *expr_ptr;
        while (*look == ' ')
            look++;

        if (*look == '(') {
            /* function call */
            /* advance expr_ptr to '(' */
            *expr_ptr = look;
            (*expr_ptr)++; /* consume '(' */

            ExprNode **args = NULL;
            int argc = 0;

            /* empty arg list */
            while (**expr_ptr == ' ')
                (*expr_ptr)++;

            if (**expr_ptr != ')') {
                for (;;) {
                    ExprNode *arg = parse_internal(expr_ptr, context);
                    if (!arg || EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
                        /* cleanup */
                        for (int i = 0; i < argc; ++i)
                            exprlib_free(args[i]);
                        free(args);
                        free(name);
                        return NULL;
                    }

                    ExprNode **tmp =
                        realloc(args, sizeof(ExprNode *) * (argc + 1));
                    if (!tmp) {
                        EXPRLIB_ERROR = EXPRLIB_ERROR_MALLOC_FAILED;
                        exprlib_free(arg);
                        for (int i = 0; i < argc; ++i)
                            exprlib_free(args[i]);
                        free(args);
                        free(name);
                        return NULL;
                    }
                    args = tmp;
                    args[argc++] = arg;

                    while (**expr_ptr == ' ')
                        (*expr_ptr)++;

                    if (**expr_ptr == ',') {
                        (*expr_ptr)++; /* consume ',' */
                        while (**expr_ptr == ' ')
                            (*expr_ptr)++;
                        continue;
                    } else if (**expr_ptr == ')') {
                        break;
                    } else {
                        /* syntax error */
                        EXPRLIB_ERROR = EXPRLIB_ERROR_SYNTAX;
                        printf("SYNTAX ERROR: %s\n", *expr_ptr);
                        printf("            : ^ ',' or ')' Expected\n");
                        for (int i = 0; i < argc; ++i)
                            exprlib_free(args[i]);
                        free(args);
                        free(name);
                        return NULL;
                    }
                }
            }

            /* consume ')' */
            if (**expr_ptr != ')') {
                /* shouldn't happen but defensive */
                EXPRLIB_ERROR = EXPRLIB_ERROR_SYNTAX;
                printf("SYNTAX ERROR: %s\n", *expr_ptr);
                printf("            : ^ Closing Paren Expected\n");
                for (int i = 0; i < argc; ++i)
                    exprlib_free(args[i]);
                free(args);
                free(name);
                return NULL;
            }
            (*expr_ptr)++; /* consume ')' */

            ExprNode *fn_node = create_function_node(name, args, argc);
            free(name);

            if (!fn_node) {
                for (int i = 0; i < argc; ++i)
                    exprlib_free(args[i]);
                free(args);
                return NULL;
            }

            return fn_node;
        }

        /* not a function call -> variable */
        if (!is_defined_variable(name, context)) {
            EXPRLIB_ERROR = EXPRLIB_ERROR_UNDEFINED_VARIABLE;
            printf("UNDEFINED VARIABLE ERROR: '%s'\n", name);
            free(name);
            return NULL;
        }

        ExprNode *node = create_variable_node(name);
        free(name);
        return node;
    }

    EXPRLIB_ERROR = EXPRLIB_ERROR_SYNTAX;
    printf("SYNTAX ERROR: %s\n", *expr_ptr);
    printf("            : ^ Unexpected Token\n");
    return NULL;
}

ExprNode *parse_binary_rhs(int expr_prec, ExprNode *lhs, const char **expr_ptr,
                           const ExprContext *context) {
    while (1) {
        while (**expr_ptr == ' ')
            (*expr_ptr)++;

        char op = **expr_ptr;
        int tok_prec = get_precedence(op);

        if (tok_prec < expr_prec)
            return lhs;

        (*expr_ptr)++;

        ExprNode *rhs = parse_unary(expr_ptr, context);
        if (!rhs) {
            exprlib_free(lhs);
            return NULL;
        }

        while (**expr_ptr == ' ')
            (*expr_ptr)++;

        int next_prec = get_precedence(**expr_ptr);
        if (tok_prec < next_prec ||
            (tok_prec == next_prec && is_right_associative(op))) {
            rhs = parse_binary_rhs(tok_prec + 1, rhs, expr_ptr, context);
            if (!rhs) {
                exprlib_free(lhs);
                return NULL;
            }
        }

        if (lhs->type == EXPR_NODE_NUMBER && rhs->type == EXPR_NODE_NUMBER) {
            double left_val = lhs->data.number;
            double right_val = rhs->data.number;
            double result;

            switch (op) {
            case '+':
                result = left_val + right_val;
                break;
            case '-':
                result = left_val - right_val;
                break;
            case '*':
                result = left_val * right_val;
                break;
            case '/':
                if (right_val == 0.0) {
                    EXPRLIB_ERROR = EXPRLIB_ERROR_DIVISION_BY_ZERO;
                    exprlib_free(lhs);
                    exprlib_free(rhs);
                    return NULL;
                }
                result = left_val / right_val;
                break;
            case '^':
                result = pow(left_val, right_val);
                break;
            default:
                EXPRLIB_ERROR = EXPRLIB_ERROR_UNKNOWN;
                exprlib_free(lhs);
                exprlib_free(rhs);
                return NULL;
            }

            exprlib_free(lhs);
            exprlib_free(rhs);
            lhs = create_number_node(result);
            if (!lhs || EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
                return NULL;
            }
            continue;
        } else {
            lhs = create_operator_node(op, lhs, rhs);
            if (!lhs || EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
                exprlib_free(rhs);
                return NULL;
            }
        }
    }
}

ExprNode *parse_internal(const char **expr_ptr, const ExprContext *context) {
    ExprNode *lhs = parse_unary(expr_ptr, context);
    if (!lhs || EXPRLIB_ERROR != EXPRLIB_SUCCESS)
        return NULL;
    return parse_binary_rhs(0, lhs, expr_ptr, context);
}

void print_expr_tree(const ExprNode *node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    print_indent(indent);

    switch (node->type) {
    case EXPR_NODE_NUMBER:
        printf("NUMBER: %g\n", node->data.number);
        break;

    case EXPR_NODE_VARIABLE:
        printf("VARIABLE: %s\n", node->data.variable_name);
        break;

    case EXPR_NODE_OPERATOR:
        printf("OPERATOR: '%c'\n", node->data.op_node.op);
        print_indent(indent);
        printf("LHS:\n");
        print_expr_tree(node->data.op_node.left, indent + 2);
        print_indent(indent);
        printf("RHS:\n");
        print_expr_tree(node->data.op_node.right, indent + 2);
        break;

    case EXPR_NODE_FUNCTION_CALL:
        printf("FUNCTION CALL: %s (argc=%d)\n", node->data.fn_call.name,
               node->data.fn_call.argc);
        for (int i = 0; i < node->data.fn_call.argc; i++) {
            print_indent(indent + 2);
            printf("ARG %d:\n", i);
            print_expr_tree(node->data.fn_call.args[i], indent + 4);
        }
    }
}

ExprNode *exprlib_parse(const string expression, const ExprContext *context) {
    EXPRLIB_ERROR = EXPRLIB_SUCCESS;
    const char *ptr = expression;
    return parse_internal(&ptr, context);
}

double evaluate_node(const ExprNode *node, const ExprContext *context) {
    if (!node) {
        EXPRLIB_ERROR = EXPRLIB_ERROR_UNKNOWN;
        return 0.0;
    }

    switch (node->type) {
    case EXPR_NODE_NUMBER:
        return node->data.number;

    case EXPR_NODE_VARIABLE: {
        for (int i = 0; i < g_constant_count; i++) {
            if (strcmp(g_constants[i].name, node->data.variable_name) == 0) {
                return g_constants[i].value;
            }
        }
        for (int i = 0; i < context->var_count; i++) {
            if (strcmp(context->variables[i].name, node->data.variable_name) ==
                0) {
                return *(context->variables[i].value);
            }
        }
        EXPRLIB_ERROR = EXPRLIB_ERROR_UNDEFINED_VARIABLE;
        printf("UNDEFINED VARIABLE ERROR: '%s'\n", node->data.variable_name);
        return 0.0;
    }

    case EXPR_NODE_OPERATOR: {
        double left_val = evaluate_node(node->data.op_node.left, context);
        if (EXPRLIB_ERROR != EXPRLIB_SUCCESS)
            return 0.0;

        double right_val = evaluate_node(node->data.op_node.right, context);
        if (EXPRLIB_ERROR != EXPRLIB_SUCCESS)
            return 0.0;

        switch (node->data.op_node.op) {
        case '+':
            return left_val + right_val;
        case '-':
            return left_val - right_val;
        case '*':
            return left_val * right_val;
        case '/':
            if (right_val == 0.0) {
                EXPRLIB_ERROR = EXPRLIB_ERROR_DIVISION_BY_ZERO;
                return 0.0;
            }
            return left_val / right_val;
        case '^':
            return pow(left_val, right_val);
        default:
            EXPRLIB_ERROR = EXPRLIB_ERROR_UNKNOWN;
            return 0.0;
        }
    }

    case EXPR_NODE_FUNCTION_CALL: {
        const ExprLibFunction *fn = find_function(node->data.fn_call.name);

        if (!fn) {
            EXPRLIB_ERROR = EXPRLIB_ERROR_UNKNOWN;
            return 0.0;
        }

        int argc = node->data.fn_call.argc;
        if (fn->arity != -1 && fn->arity != argc) {
            EXPRLIB_ERROR = EXPRLIB_ERROR_SYNTAX;
            printf("SYNTAX ERROR: Function '%s' expects %d arguments, got %d\n",
                   fn->name, fn->arity, argc);
            return 0.0;
        }

        double *argv = alloca(sizeof(double) * argc);
        for (int i = 0; i < argc; ++i)
            argv[i] = evaluate_node(node->data.fn_call.args[i], context);

        return fn->fn(argv, argc);
    }

    default:
        EXPRLIB_ERROR = EXPRLIB_ERROR_UNKNOWN;
        return 0.0;
    }
}

double exprlib_evaluate(const ExprNode *expr, const ExprContext *context) {
    EXPRLIB_ERROR = EXPRLIB_SUCCESS;
    return evaluate_node(expr, context);
}

void exprlib_init(void) {
    g_functions = NULL;
    g_function_count = 0;
    exprlib_register_builtins();
}