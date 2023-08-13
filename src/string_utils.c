#include "include/string_utils.h"

#include <stdlib.h>
#include <string.h>

int is_digit(int ch) { return ch >= '0' && ch <= '9'; }

int is_whitespace(int ch) {
    return ch == ' ' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v';
}

const char *scan_while(int (*predicate)(int), const char *str) {
    char *base = NULL;
    size_t len = 0;
    while (predicate(*str)) {
        char *tmp = realloc(base, (++(len) + 1) * sizeof(char));
        if (!tmp) {
            if (base) free(base);
            return NULL;
        }

        base = tmp;
        base[len - 1] = *str;

        ++str;
    }

    base[len] = '\0';
    return base;
}
