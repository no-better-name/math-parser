#include <math.h>
#include <stdio.h>

#include "include/calc.h"
#include "include/parse.h"

double calculate_expression(const char *eqn, double x) {
    double *arr = NULL;
    size_t len = 0;
    void *tmp;
    TokenStack parse = parse_expression(eqn);

    while (parse.len) {
        Token token = pop_back(&parse);
        if (token.kind == kTokenLiteral) {
            tmp = realloc(arr, (len + 1) * sizeof(double));
            if (!tmp) {
                token_stack_free(&parse);
                if (token.kind == kTokenLiteral) free((void *)token.base);
                return 0;
            }

            arr = tmp;
            sscanf(token.base, "%lf", arr + len++);
            continue;
        }

        if (token.kind == kTokenIdentifier) {
            tmp = realloc(arr, (len + 1) * sizeof(double));
            if (!tmp) {
                token_stack_free(&parse);
                if (token.kind == kTokenLiteral) free((void *)token.base);
                return 0;
            }

            arr = tmp;
            arr[len++] = x;
            continue;
        }

        if (token.kind == kTokenUnaryOperator) {
            arr[len - 1] = -arr[len - 1];
            continue;
        }

        if (token.kind == kTokenBinaryOperator) {
            switch (*token.base) {
                case '+':
                    arr[len - 2] += arr[len - 1];
                    arr = realloc(arr, (--len) * sizeof(double));
                    break;

                case '-':
                    arr[len - 2] -= arr[len - 1];
                    arr = realloc(arr, (--len) * sizeof(double));
                    break;

                case '*':
                    arr[len - 2] *= arr[len - 1];
                    arr = realloc(arr, (--len) * sizeof(double));
                    break;

                case '/':
                    arr[len - 2] /= arr[len - 1];
                    arr = realloc(arr, (--len) * sizeof(double));
                    break;
            }
        }

        if (token.kind == kTokenFunction) {
            if (token.base == funcs[0]) {
                arr[len - 1] = cos(arr[len - 1]);
            }

            if (token.base == funcs[1]) {
                arr[len - 1] = 1.0 / tan(arr[len - 1]);
            }

            if (token.base == funcs[2]) {
                arr[len - 1] = log(arr[len - 1]);
            }

            if (token.base == funcs[3]) {
                arr[len - 1] = sin(arr[len - 1]);
            }

            if (token.base == funcs[4]) {
                arr[len - 1] = sqrt(arr[len - 1]);
            }

            if (token.base == funcs[5]) {
                arr[len - 1] = tan(arr[len - 1]);
            }
            continue;
        }
    }

    double result = arr[0];
    free(arr);
    return result;
}
