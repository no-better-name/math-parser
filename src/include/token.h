#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

#define MAX_TOKEN_KINDS 7
#define MAX_FUNCS 6

extern const char *const funcs[MAX_FUNCS];        // cos ctg ln sin sqrt tan

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

enum {
    kFuncCos = 0,
    kFuncCtg,
    kFuncLn,
    kFuncSin,
    kFuncSqrt,
    kFuncTan,
};

typedef struct {
    int kind;
    union {
        struct {
            size_t a, b;
        } __bits;
        struct {
            double val;
            size_t len;
        };
        char id;
        char op;
        int func;
    };
} Token;

Token read_next_token(const char *base);

Token read_literal(const char *str);
Token read_func(const char *str);
Token read_binary(const char *ch);
Token read_unary(const char *ch);
Token read_id(const char *ch);
Token read_paren_left(const char *ch);
Token read_paren_right(const char *ch);

extern Token (*const read_token_kind[MAX_TOKEN_KINDS])(const char *);

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
