#include "exprlib.h"

int main() {
    exprlib_init();
    string expr = "e^x * sin(x)";
    double x_value = 5.0;
    ExprLibVariable vars[] = {{"x", &x_value}};
    ExprContext context = {vars, 1};
    ExprNode *parsed_expr = exprlib_parse(expr, &context);
    if (EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
        printf("Error parsing expression: %s\n",
               EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
        return 1;
    }
    print_expr_tree(parsed_expr, 0);
    x_value = 10;
    double result = exprlib_evaluate(parsed_expr, &context);
    if (EXPRLIB_ERROR != EXPRLIB_SUCCESS) {
        printf("Error evaluating expression: %s\n",
               EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
    } else {
        printf("Result: %g\n", result);
    }
    free_expr(parsed_expr);
    return 0;
}