#include "include/token.h"
#include "include/string_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *const funcs[MAX_FUNCS] = {"cos", "ctg", "ln", "sin", "sqrt", "tan"};

Token (*const read_token_kind[MAX_TOKEN_KINDS])(const char *) = {read_literal, read_id, read_unary, read_binary, read_func, read_paren_left, read_paren_right};

Token read_next_token(const char *base) {
    Token token = {.kind = kTokenEOF};
    static const char *eqn;
    static int state_current, token_read = kTokenEOF;
    if (base) {
        eqn = base;
        state_current = kReadExpr;
    }

    if (!eqn) return token;

    while (is_whitespace(*(eqn))) ++eqn;
    if (!*eqn) {
        if (token_read & (kReadOperationOrLeftParen | kReadFunc)) token.kind = kTokenInvalid;
        eqn = NULL;
        return token;
    }

    token_read = kTokenLiteral;
    size_t index = 0;
    // for (; !(token_read & kTokenInvalid) || !(state_current & token_read) || (token = read_token_kind[index](eqn), token.kind == kTokenInvalid); ++index, token_read <<= 1);
    while (token_read != kTokenInvalid) {
        if (state_current & token_read) {
            token = read_token_kind[index](eqn);
            if (token.kind == token_read) break;
        }

        token_read <<= 1;
        ++index;
    }

    if (token_read & kTokenInvalid) {
        token.kind = kTokenInvalid;
        eqn = NULL;
        return token;
    }

    if (token.kind & kReadNonFuncOperandOrRightParen) state_current = kReadAfterNonFuncExpr;
    if (token.kind & kReadOperationOrLeftParen) state_current = kReadExpr;
    if (token.kind & kReadFunc) state_current = kTokenLeftParenthesis;

    if (token.kind & (kReadOperationOrLeftParen | kTokenRightParenthesis | kTokenIdentifier)) {
        ++eqn;
    } else if (token.kind & kTokenFunction) {
        eqn += strlen(funcs[token.func]);
    } else {
        eqn += token.len;
    }

    return token;
}

Token read_literal(const char *str) {
    Token token = {.kind = kTokenInvalid};
    if (!is_digit(*str)) return token;
    int count = 0, scan_result;
    double val;
    scan_result = sscanf(str, "%lg%n", &val, &count);
    if (scan_result == 1 && count) {
        token.kind = kTokenLiteral;
        token.val = val;
        token.len = count;
    }

    return token;
}

Token read_func(const char *str) {
    Token token = {.kind = kTokenInvalid};
    for (size_t index = 0; index < MAX_FUNCS; ++index) {
        if (!strncmp(str, funcs[index], strlen(funcs[index]))) {
            token.kind = kTokenFunction;
            token.func = index;
        }
    }

    return token;
}

Token read_binary(const char *ch) {
    Token token = {.kind = kTokenInvalid};
    switch (*ch) {
        case '-':
        case '+':
        case '/':
        case '*':
            token.kind = kTokenBinaryOperator;
            token.op = *ch;
            break;
    }

    return token;
}

Token read_unary(const char *ch) {
    Token token = {.kind = kTokenInvalid};
    switch (*ch) {
        case '-':
            token.kind = kTokenUnaryOperator;
            token.op = *ch;
            break;
    }

    return token;
}

Token read_id(const char *ch) {
    Token token = {.kind = kTokenInvalid};
    switch (*ch) {
        case 'x':
            token.kind = kTokenIdentifier;
            token.id = *ch;
            break;
    }

    return token;
}

Token read_paren_left(const char *ch) {
    Token token = {.kind = kTokenInvalid};
    switch (*ch) {
        case '(':
            token.kind = kTokenLeftParenthesis;
            break;
    }

    return token;
}

Token read_paren_right(const char *ch) {
    Token token = {.kind = kTokenInvalid};
    switch (*ch) {
        case ')':
            token.kind = kTokenRightParenthesis;
            break;
    }

    return token;
}

Token *push_back(TokenStack *stack, Token token) {
    if (!stack) return NULL;

    void *tmp = realloc(stack->base, (stack->len + 1) * sizeof(Token));
    if (!tmp) return NULL;
    stack->base = tmp;
    stack->base[stack->len] = token;
    ++stack->len;

    return stack->base + stack->len - 1;
}

Token *push_front(TokenStack *stack, Token token) {
    if (!stack) return NULL;

    void *tmp = realloc(stack->base, (stack->len + 1) * sizeof(Token));
    if (!tmp) return NULL;
    stack->base = tmp;
    memmove(stack->base + 1, stack->base, stack->len * sizeof(Token));
    stack->base[0] = token;
    ++stack->len;

    return stack->base;
}

Token pop_back(TokenStack *stack) {
    Token invalid = {.kind = kTokenInvalid};
    if (!stack || !stack->base) return invalid;

    Token elem = stack->base[stack->len - 1];
    if (stack->len - 1) {
        stack->base = realloc(stack->base, (stack->len - 1) * sizeof(Token));
    } else {
        free(stack->base);
        stack->base = NULL;
    }
    --stack->len;

    return elem;
}

Token pop_front(TokenStack *stack) {
    Token invalid = {.kind = kTokenInvalid};
    if (!stack || !stack->base) return invalid;

    Token elem = stack->base[0];
    if (stack->len - 1) {
        memmove(stack->base, stack->base + 1, (stack->len - 1) * sizeof(Token));
        stack->base = realloc(stack->base, (stack->len - 1) * sizeof(Token));
    } else {
        free(stack->base);
        stack->base = NULL;
    }
    --stack->len;

    return elem;
}

TokenStack generate_stack(const char *eqn, int direction) {
    TokenStack stack = {.base = NULL, .len = 0};
    Token token;
    void *ptr = NULL;

    switch (direction) {
        case kPushBack:
            token = read_next_token(eqn);
            while (token.kind && token.kind != kTokenInvalid) {
                ptr = push_back(&stack, token);
                if (!ptr) {
                    if (stack.base) free(stack.base);
                    return stack;
                }
                token = read_next_token(NULL);
            }

            ptr = push_back(&stack, token);
            if (!ptr) {
                if (stack.base) free(stack.base);
                return stack;
            }
            break;

        case kPushFront:
            token = read_next_token(eqn);
            while (token.kind && token.kind != kTokenInvalid) {
                ptr = push_front(&stack, token);
                if (!ptr) {
                    if (stack.base) free(stack.base);
                    return stack;
                }
                token = read_next_token(NULL);
            }

            ptr = push_front(&stack, token);
            if (!ptr) {
                if (stack.base) free(stack.base);
                return stack;
            }
            break;
    }

    return stack;
}

Token *peek_back(const TokenStack *stack) {
    if (!stack || !stack->base) return NULL;
    return stack->base + stack->len - 1;
}

Token *peek_front(const TokenStack *stack) {
    if (!stack || !stack->base) return NULL;
    return stack->base;
}

void token_stack_free(TokenStack *stack) {
    if (stack->base) free(stack->base);
    stack->base = NULL;
    stack->len = 0;
}

int is_left_associative(const Token* token) {
    if (token->kind == kTokenBinaryOperator) {
        return token->op == '-' || token->op == '/';
    }

    return 0;
}
