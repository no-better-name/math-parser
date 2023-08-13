#ifndef PARSE_H
#define PARSE_H

#include "token.h"

TokenStack parse_expression(const char *eqn);

int precedence(const Token *token);

#endif
