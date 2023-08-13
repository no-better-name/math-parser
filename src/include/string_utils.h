#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>

int is_digit(int ch);
int is_whitespace(int ch);
const char *scan_while(int (*predicate)(int), const char *str);

#endif
