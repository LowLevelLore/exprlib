# ExprLib — C Expression Evaluator


A small, embeddable expression parser/evaluator in C.

Features:

* AST-based parsing and evaluation
* Numeric literals, variables, binary operators, unary `-`
* Function-call nodes with built-in math functions (`sin`, `cos`, `tan`, `sqrt`, `log`, `pow`, `floor`, `ceil`, …)
* Runtime registration of user-defined functions
* Simple error reporting via `ExprLibError`

This README shows installation, build, API usage, examples and maintenance notes in the chronological order you need to get started.

## Prerequisites

* C compiler (gcc/clang) with C99+ support
* `make` or CMake (optional)
* `libm` (usually provided by standard C runtime)
* A POSIX-like shell for example commands

## Installation

Add the [exprlib.h](https://github.com/LowLevelLore/exprlib/blob/main/include/exprlib.h) and [exprlib.c](https://github.com/LowLevelLore/exprlib/blob/main/src/exprlib.c) in your C or C++ project's build system and compile along.

## Quickstart — API & Usage

**Public Functions:**

```C
/* All the functions except exprlib_init() and exprlib_free() set EXPRLIB_ERROR in case of any error */

void exprlib_init(void);
void exprlib_free(ExprNode *node);

/* parse expression into an AST (returns NULL on error) */
ExprNode *exprlib_parse(const string expression, const ExprContext *context);

/* evaluate AST (returns numeric result, sets EXPRLIB_ERROR on failure) */
double exprlib_evaluate(const ExprNode *expr, const ExprContext *context);

/* register user function at runtime */
bool exprlib_register_function(const char *name, int arity, ExprLibFnPtr fn);
```

**Data Structures:**

```c
typedef struct {
    string name; /* variable name (ownership or copy semantics handled by caller) */
    double *value; /* pointer to variable's double value */
} ExprLibVariable;

typedef struct {
    ExprLibVariable *variables;
    int var_count;
} ExprContext;
```


## Usage

1. **Evaluate a constant expression**

   ```C
   #include <stdio.h>
   #include "exprlib.h"

   int main(void) {
       exprlib_init();
       ExprNode *ast = exprlib_parse("2 + 3 * 4", NULL);
       if (!ast) {
           fprintf(stderr, "parse failed: %s\n", EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
           return 1;
       }
       double r = exprlib_evaluate(ast, NULL);
       printf("result = %f\n", r); /* expected 14.0 */
       free_expr(ast);
       return 0;
   }

   ```
2. **Evaluate expression with variables**

   ```c
   #include <stdio.h>
   #include "exprlib.h"

   int main(void) {
       exprlib_init();
       double xval = 2.0, yval = 3.0;
       ExprLibVariable vars[] = {
           { "x", &xval },
           { "y", &yval }
       };
       ExprContext ctx = { vars, 2 };

       ExprNode *ast = exprlib_parse("x ^ 2 + y", &ctx);
       if (!ast) {
           fprintf(stderr, "parse failed: %s\n", EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
           return 1;
       }
       double r = exprlib_evaluate(ast, &ctx);
       printf("result = %f\n", r); /* expected 7.0 */
       free_expr(ast);
       return 0;
   }

   ```
3. **Use built-in math functions**

   ```c
   #include <stdio.h>
   #include "exprlib.h"

   int main(void) {
       exprlib_init();
       ExprNode *ast = exprlib_parse("sin(pi / 2) + sqrt(16)", NULL);
       if (!ast) {
           fprintf(stderr, "parse failed: %s\n", EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
           return 1;
       }
       double r = exprlib_evaluate(ast, NULL);
       printf("result = %f\n", r); /* ~1 + 4 = 5 */
       free_expr(ast);
       return 0;
   }

   ```
4. **Register a user function**

   ```c
   #include <stdio.h>
   #include <math.h>
   #include "exprlib.h"

   /* custom function: max(a, b) */
   static double fn_max(const double *a, int n) {
       assert(n == 2);
       return a[0] > a[1] ? a[0] : a[1];
   }

   int main(void) {
       exprlib_init();
       /* register at runtime */
       if (!exprlib_register_function("max", 2, fn_max)) {
           fprintf(stderr, "failed to register function: %s\n", EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
           return 1;
       }

       ExprNode *ast = exprlib_parse("max(10, sqrt(25)) + 1", NULL);
       if (!ast) {
           fprintf(stderr, "parse failed: %s\n", EXPRLIB_ERROR_MESSAGES[EXPRLIB_ERROR]);
           return 1;
       }
       double r = exprlib_evaluate(ast, NULL);
       printf("result = %f\n", r); /* expected max(10,5)+1 = 11 */
       free_expr(ast);

       /* cleanup */
       exprlib_clear_functions();
       return 0;
   }
   ```
5. **Error handling examples**

   a. **Undefined variable** : parsing or evaluating `a + 2` without `a` in `ExprContext` sets `EXPRLIB_ERROR_UNDEFINED_VARIABLE`.

   b. **Syntax error** : malformed input sets `EXPRLIB_ERROR_SYNTAX`.

    c.**Memory allocation failure** : sets `EXPRLIB_ERROR_MALLOC_FAILED`.

Always check parse result for `NULL`, and check `EXPRLIB_ERROR` for error detail.

## Built-in functions (summary)

The library ships with a comprehensive set of math functions and constants suitable for high-school and entrance-exam–level mathematics (Class 11–12 / JEE).

### Trigonometric

- `sin(x)`
- `cos(x)`
- `tan(x)`
- `cot(x)`
- `sec(x)`
- `cosec(x)`

### Inverse Trigonometric

- `asin(x)`
- `acos(x)`
- `atan(x)`

### Powers and Roots

- `pow(x, y)`
- `sqrt(x)`
- `cbrt(x)`

### Logarithmic and Exponential

- `ln(x)` (natural logarithm)
- `log10(x)`
- `exp(x)`

### Absolute and Rounding

- `abs(x)`
- `floor(x)`
- `ceil(x)`
- `round(x)`

### Angle Conversion

- `deg2rad(x)` — degrees to radians
- `rad2deg(x)` — radians to degrees

### Min / Max

- `min(a, b, ...)` — variadic
- `max(a, b, ...)` — variadic

### Combinatorics

- `factorial(n)`
- `nCr(n, r)`
- `nPr(n, r)`

---

## Built-in constants

The following mathematical constants are preloaded during `exprlib_init()` and can be used directly in expressions.

### Fundamental Constants

- `pi` — π
- `e` — Euler’s number

### Derived Constants

- `tau` — 2π
- `phi` — golden ratio ( (1 + √5) / 2 )

### Square Roots

- `sqrt2` — √2
- `sqrt3` — √3
- `sqrt5` — √5

### Logarithmic Constants

- `ln2` — ln(2)
- `ln10` — ln(10)
- `log2e` — log₂(e)
- `log10e` — log₁₀(e)

### Inverse Pi Multiples

- `invpi` — 1 / π
- `inv2pi` — 1 / (2π)

---

### Example usage

```text
sin(pi / 6) + cos(30 * deg2rad(1))
sqrt2 * sqrt2
max(10, sqrt(25), factorial(3))
nCr(5, 2) + nPr(4, 2)
```



## Implementation notes / best practices

* **Ownership** : `create_*` helpers return a fully-initialized node on success and never return a partially-initialized node. On success the node owns substructures passed to it (e.g., `args` array for function nodes). On failure the caller retains ownership and must free.
* **Memory cleanup** : always call `free_expr(ast)` for ASTs returned by `exprlib_parse`.
* **Thread-safety** : function registry is not thread-safe. If your app calls `exprlib_register_function` from multiple threads, protect registration with a mutex or register functions at startup only.
* **Arity** : functions use fixed arity (non-variadic). Use `arity == -1` if you implement variadic dispatch; otherwise registry checks arity strictly.
* **Domain errors** : math-domain issues propagate as `NaN` or `inf`. If you want explicit domain errors, add checks in function wrappers.

## Contributing

1. Fork the repo.
2. Implement feature or fix in a branch.
3. Follow repo coding style (use `clang-format`).
4. Add tests for non-trivial functionality.
5. Open pull request with description of changes and rationale.

## License

MIT License — see `LICENSE` file.
