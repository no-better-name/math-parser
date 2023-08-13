#include "include/parse.h"

int precedence(const Token *token) {
    if (token->kind == kTokenBinaryOperator) {
        if (*token->base == '+' || *token->base == '-') return 0;
        if (*token->base == '*' || *token->base == '/') return 1;
    }

    if (token->kind == kTokenUnaryOperator) return 2;
    if (token->kind == kTokenFunction) return 3;

    return -1;
}

TokenStack parse_expression(const char *eqn) {
    TokenStack tokens = generate_stack(eqn, kPushFront);
    TokenStack output = {.base = NULL, .len = 0};
    TokenStack operators = {.base = NULL, .len = 0};
    if (pop_front(&tokens).kind & kTokenInvalid) return output;

    while (tokens.len) {
        Token token = pop_back(&tokens);
        if (token.kind == kTokenLiteral || token.kind == kTokenIdentifier) {
            push_front(&output, token);
            continue;
        }

        if (token.kind == kTokenFunction) {
            push_front(&operators, token);
            continue;
        }

        if (token.kind & (kTokenBinaryOperator | kTokenUnaryOperator)) {
            while (operators.len && peek_front(&operators)->kind != kTokenLeftParenthesis
                    && (precedence(peek_front(&operators)) > precedence(&token)
                        || (precedence(peek_front(&operators)) == precedence(&token) && is_left_associative(&token)))) {
                push_front(&output, pop_front(&operators));
            }
            push_front(&operators, token);
            continue;
        }

        if (token.kind == kTokenLeftParenthesis) {
            push_front(&operators, token);
            continue;
        }

        if (token.kind == kTokenRightParenthesis) {
            while (operators.len && peek_front(&operators)->kind != kTokenLeftParenthesis) {
                push_front(&output, pop_front(&operators));
            }

            if (!operators.len) {
                token_stack_free(&tokens);
                token_stack_free(&output);
                token_stack_free(&operators);

                token.kind = kTokenEOF;
                token.base = NULL;

                push_back(&output, token);

                return output;
            }
            pop_front(&operators);

            if (operators.len && peek_front(&operators)->kind == kTokenFunction) {
                push_front(&output, pop_front(&operators));
            }

            continue;
        }
    }

    while (operators.len) {
        if (peek_front(&operators)->kind & (kTokenLeftParenthesis | kTokenRightParenthesis)) {
            token_stack_free(&tokens);
            token_stack_free(&output);
            token_stack_free(&operators);

            Token token = {.kind = kTokenEOF, .base = NULL};
            push_back(&output, token);

        }

        push_front(&output, pop_front(&operators));
    }

    return output;
}
