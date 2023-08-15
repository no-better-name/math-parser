#include <stdio.h>

#include "include/calc.h"
#include "include/parse.h"
#include "include/token.h"

int token_print(const Token *token) {
    int chars_printed = 0;
    chars_printed += fprintf(stderr, "token %d", token->kind);

    switch (token->kind) {
        case kTokenIdentifier:
            chars_printed += fprintf(stderr, " (id): %c", token->id);
            break;
        case kTokenLiteral:
            chars_printed += fprintf(stderr, " (literal): %lg", token->val);
            break;
        case kTokenUnaryOperator:
            chars_printed += fprintf(stderr, " (unary op): %c", token->op);
            break;
        case kTokenBinaryOperator:
            chars_printed += fprintf(stderr, " (binary op): %c", token->op);
            break;
        case kTokenFunction:
            chars_printed += fprintf(stderr, " (func): %s", funcs[token->func]);
            break;
        case kTokenLeftParenthesis:
            chars_printed += fprintf(stderr, " (left paren): (");
            break;
        case kTokenRightParenthesis:
            chars_printed += fprintf(stderr, " (right paren): )");
            break;
        case kTokenEOF:
            chars_printed += fprintf(stderr, " <END OF EXPRESSION>");
            break;
        case kTokenInvalid:
            chars_printed += fprintf(stderr, " <TOKEN INVALID>");
            break;
    }

    return chars_printed;
}

int main(void) {
    char *eqn;
    double x = 1;
    scanf("%m[^\n]", &eqn);

    Token result = calculate_expression(eqn, x);
    if (result.kind != kTokenInvalid) {
        token_print(&result);
        putchar('\n');
    }

    free(eqn);
}
