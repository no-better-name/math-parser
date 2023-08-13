#include <stdio.h>

#include "include/calc.h"
#include "include/parse.h"

int token_print(const Token *token) { return fprintf(stderr, "token %d: \"%s\"", token->kind, token->base); }

int main(void) {
    char *eqn;
    scanf("%m[^\n]", &eqn);

    double val = calculate_expression(eqn, 1);
    printf("%s = %lf\n", eqn, val);

    free(eqn);
}
