#include "include/string_utils.h"

#include <stdlib.h>
#include <string.h>

int is_digit(int ch) { return ch >= '0' && ch <= '9'; }

int is_whitespace(int ch) {
    return ch == ' ' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v';
}
