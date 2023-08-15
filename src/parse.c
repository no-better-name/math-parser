#include "include/parse.h"
#include "include/token.h"

int precedence(const Token *token) {
    if (token->kind == kTokenBinaryOperator) {
        if (token->op == '+' || token->op == '-') return 0;
        if (token->op == '*' || token->op == '/') return 1;
    }

    if (token->kind == kTokenUnaryOperator) return 2;
    if (token->kind == kTokenFunction) return 3;

    return -1;
}

TokenStack parse_expression(const char *eqn) {
    TokenStack tokens = generate_stack(eqn, kPushFront);
    TokenStack output = {.base = NULL, .len = 0};
    TokenStack operators = {.base = NULL, .len = 0};
    if (tokens.len && (peek_front(&tokens)->kind & kTokenInvalid)) {
        push_back(&output, pop_front(&tokens));
        token_stack_free(&tokens);
        return output;
    }
    if (tokens.len && (peek_front(&tokens)->kind & kTokenEOF)) pop_front(&tokens);

    while (tokens.len) {
        Token token = pop_back(&tokens);
        switch (token.kind) {
            case kTokenLiteral:
            case kTokenIdentifier:
                push_front(&output, token);
                break;
            case kTokenFunction:
            case kTokenLeftParenthesis:
                push_front(&operators, token);
                break;
            case kTokenUnaryOperator:
            case kTokenBinaryOperator:
                while (operators.len && peek_front(&operators)->kind != kTokenLeftParenthesis
                        && (precedence(peek_front(&operators)) > precedence(&token)
                            || (precedence(peek_front(&operators)) == precedence(&token) && is_left_associative(&token)))) {
                    push_front(&output, pop_front(&operators));
                }
                push_front(&operators, token);
                break;
            case kTokenRightParenthesis:
                while (operators.len && peek_front(&operators)->kind != kTokenLeftParenthesis) {
                    push_front(&output, pop_front(&operators));
                }

                if (!operators.len) {
                    token_stack_free(&tokens);
                    token_stack_free(&output);
                    token_stack_free(&operators);

                    token.kind = kTokenInvalid;
                    token.__bits.a = 0ULL;
                    token.__bits.b = 0ULL;

                    push_back(&output, token);

                    return output;
                }
                pop_front(&operators);

                if (operators.len && peek_front(&operators)->kind == kTokenFunction) {
                    push_front(&output, pop_front(&operators));
                }
                break;
        }
    }

    while (operators.len) {
        if (peek_front(&operators)->kind & (kTokenLeftParenthesis | kTokenRightParenthesis)) {
            token_stack_free(&tokens);
            token_stack_free(&output);
            token_stack_free(&operators);

            Token token = {.kind = kTokenInvalid};
            push_back(&output, token);

            return output;
        }

        push_front(&output, pop_front(&operators));
    }

    return output;
}
