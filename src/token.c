#include "include/token.h"

#include <stdlib.h>
#include <string.h>

#include "include/string_utils.h"

const char *const funcs[6] = {"cos", "ctg", "ln", "sin", "sqrt", "tan"};
const char *const binary_ops[4] = {"+", "-", "*", "/"};
const char *const unary_ops[1] = {"-"};
const char *const identifiers[1] = {"x"};
const char *const parens[2] = {"(", ")"};
const size_t max_func_len = 4;

const char *(*read_token_kind[MAX_TOKEN_KINDS])(const char *) = {
    read_literal, read_id, read_unary, read_binary, read_func, read_left_paren, read_right_paren};

const char *read_literal(const char *str) {
    if (!str || !is_digit(*str) || *str == '0') return NULL;

    return scan_while(is_digit, str);
}

const char *read_func(const char *str) {
    // cos ctg ln sin sqrt tan

    for (size_t index = 0; index < sizeof(funcs) / sizeof(char *); ++index) {
        if (!strncmp(str, funcs[index], strlen(funcs[index]))) return funcs[index];
    }

    return NULL;
}

const char *read_binary(const char *ch) {
    if (!ch) return NULL;

    if (*ch == '+') return binary_ops[0];
    if (*ch == '-') return binary_ops[1];
    if (*ch == '*') return binary_ops[2];
    if (*ch == '/') return binary_ops[3];

    return NULL;
}

const char *read_unary(const char *ch) {
    if (!ch) return NULL;

    if (*ch == '-') return unary_ops[0];

    return NULL;
}

const char *read_id(const char *ch) {
    if (!ch) return NULL;

    if (*ch == 'x') return identifiers[0];

    return NULL;
}

const char *read_left_paren(const char *ch) {
    if (!ch) return NULL;

    if (*ch == '(') return parens[0];

    return NULL;
}

const char *read_right_paren(const char *ch) {
    if (!ch) return NULL;

    if (*ch == ')') return parens[1];

    return NULL;
}

Token read_next_token(const char *base) {
    Token token = {.kind = kTokenEOF, .base = NULL};
    static const char *str;
    static int state, next_read = kTokenEOF;

    if (base) {
        str = base;
        state = kReadExpr;
    }

	if (!str) return token;
    while (is_whitespace(*str)) ++str;

	if (!*str) {
		if (!(next_read & kReadNonFuncOperandOrRightParen)) {
			str = NULL;
			token.kind = kTokenInvalid;
		}

		return token;
	}

    const char *next_contents = NULL;
    next_read = kTokenLiteral;
    for (size_t index = 0; index < MAX_TOKEN_KINDS; ++index) {
        next_contents = read_token_kind[index](str);
        if (next_contents && (state & next_read)) {
            token.kind = next_read;
            token.base = next_contents;
            break;
        }

        next_read <<= 1;
    }

    if (next_read & kReadNonFuncOperandOrRightParen) state = kReadAfterNonFuncExpr;
    if (next_read & kReadOperationOrLeftParen) state = kReadExpr;
    if (next_read & kReadFunc) state = kTokenLeftParenthesis;

    if (next_read & kTokenInvalid) {
        str = NULL;
        token.kind = kTokenInvalid;
    } else {
        str += strlen(token.base);
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
    Token invalid = {.kind = kTokenInvalid, .base = NULL};
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
    Token invalid = {.kind = kTokenInvalid, .base = NULL};
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

void token_stack_free(TokenStack *stack) {
    while (stack->len) {
        Token token = pop_back(stack);
        if (token.kind == kTokenLiteral) free((void *)token.base);
    }
}

TokenStack generate_stack(const char *eqn, int direction) {
    TokenStack stack = {.base = NULL, .len = 0};

    switch (direction) {
        case kPushBack:
            push_back(&stack, read_next_token(eqn));
            while (stack.base[stack.len - 1].kind != kTokenEOF
                    || stack.base[stack.len - 1].kind != kTokenInvalid) {
                push_back(&stack, read_next_token(NULL));
            }
            break;

        case kPushFront:
            push_front(&stack, read_next_token(eqn));
            while (stack.base[0].kind != kTokenEOF
                    && stack.base[0].kind != kTokenInvalid) {
                push_front(&stack, read_next_token(NULL));
            }
            break;
    }

    return stack;
}

Token *peek_back(const TokenStack *stack) {
    return stack->base + stack->len - 1;
}

Token *peek_front(const TokenStack *stack) {
    return stack->base;
}

int is_left_associative(const Token* token) {
    return *token->base == '-' || *token->base == '/';
}
