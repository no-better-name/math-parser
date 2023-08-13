#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

#define MAX_TOKEN_KINDS 7

extern const char *const funcs[6];        // cos ctg ln sin sqrt tan
extern const char *const binary_ops[4];   // + - * /
extern const char *const unary_ops[1];    // -
extern const char *const identifiers[1];  // x
extern const char *const parens[2];
extern const size_t max_func_len;

extern const char *(*read_token_kind[MAX_TOKEN_KINDS])(const char *);

const char *read_literal(const char *str);
const char *read_func(const char *str);
const char *read_binary(const char *ch);
const char *read_unary(const char *ch);
const char *read_id(const char *ch);
const char *read_left_paren(const char *ch);
const char *read_right_paren(const char *ch);

enum {
    kTokenLiteral = 0x01,
    kTokenIdentifier = 0x02,

    kTokenUnaryOperator = 0x04,
    kTokenBinaryOperator = 0x08,

    kTokenFunction = 0x10,

    kTokenLeftParenthesis = 0x20,
    kTokenRightParenthesis = 0x40,

    kTokenEOF = 0x00,
    kTokenInvalid = 0x80,
};

enum {
    kReadExpr =
        kTokenLiteral | kTokenIdentifier | kTokenLeftParenthesis | kTokenUnaryOperator | kTokenFunction,
    kReadNonFuncOperandOrRightParen = kTokenLiteral | kTokenIdentifier | kTokenRightParenthesis,
    kReadOperationOrLeftParen = kTokenUnaryOperator | kTokenBinaryOperator | kTokenLeftParenthesis,
    kReadFunc = kTokenFunction,
    kReadAfterNonFuncExpr = kTokenBinaryOperator | kTokenRightParenthesis,
};

enum {
    kPushBack = 0,
    kPushFront,
};

typedef struct {
    int kind;
    const char *base;
} Token;

Token read_next_token(const char *base);

typedef struct {
    Token *base;
    size_t len;
} TokenStack;

Token *push_back(TokenStack *stack, Token token);
Token *push_front(TokenStack *stack, Token token);

Token pop_back(TokenStack *stack);
Token pop_front(TokenStack *stack);

void token_stack_free(TokenStack *stack);

TokenStack generate_stack(const char *eqn, int direction);

Token *peek_back(const TokenStack *stack);
Token *peek_front(const TokenStack *stack);

int is_left_associative(const Token* token);

#endif
