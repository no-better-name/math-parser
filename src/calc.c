#include <math.h>
#include <stdio.h>

#include "include/calc.h"
#include "include/parse.h"
#include "include/token.h"

Token calculate_expression(const char *eqn, double x) {
    TokenStack stack = parse_expression(eqn);
    TokenStack output = {.base = NULL, .len = 0};
    if (stack.len && (peek_back(&stack)->kind & kTokenInvalid)) {
        token_stack_free(&stack);
        Token token = {.kind = kTokenInvalid};
        return token;
    }
    while (stack.len) {
        Token token = pop_back(&stack);
        Token operand;
        switch (token.kind) {
            case kTokenIdentifier:
                token.kind = kTokenLiteral;
                token.val = x;
            case kTokenLiteral:
                push_back(&output, token);
                break;
            case kTokenUnaryOperator:
                peek_back(&output)->val = -peek_back(&output)->val;
                break;
            case kTokenBinaryOperator:
                operand = pop_back(&output);
                switch (token.op) {
                    case '+':
                        peek_back(&output)->val += operand.val;
                        break;
                    case '-':
                        peek_back(&output)->val -= operand.val;
                        break;
                    case '*':
                        peek_back(&output)->val *= operand.val;
                        break;
                    case '/':
                        peek_back(&output)->val /= operand.val;
                        break;
                }
                break;
            case kTokenFunction:
                switch (token.func) {
                    case kFuncCos:
                        peek_back(&output)->val = cos(peek_back(&output)->val);
                        break;
                    case kFuncCtg:
                        peek_back(&output)->val = 1.0 / tan(peek_back(&output)->val);
                        break;
                    case kFuncLn:
                        peek_back(&output)->val = log(peek_back(&output)->val);
                        break;
                    case kFuncSin:
                        peek_back(&output)->val = sin(peek_back(&output)->val);
                        break;
                    case kFuncSqrt:
                        peek_back(&output)->val = sqrt(peek_back(&output)->val);
                        break;
                    case kFuncTan:
                        peek_back(&output)->val = tan(peek_back(&output)->val);
                        break;
                }
                break;
            case kTokenInvalid:
                token_stack_free(&stack);
                return token;
        }
    }

    if (stack.len) {
        token_stack_free(&stack);
        Token token = {.kind = kTokenInvalid};
        return token;
    }

    Token token = *peek_back(&output);
    token_stack_free(&output);
    return token;
}
