#include <stdio.h>
#include <string.h>

#define REGEXP_PRINT_COMPILATION_STATUSES
#include "RegExp.h"

int main() {
    char* str = "W2";

    RegExp* expr = RegExp_create("W\\d?\\d?2$");
    RegExp_printExpression(expr);
    RegExpSearchHit hit;

    int hits = RegExp_search(expr, str, &hit);

    if(hits) {
        printf("S: %zu, L: %zu\n", hit.start, hit.length);
        printf("Match: '");
        RegExp_printSearchHit(str, &hit);
        printf("'\n");
    } else {
        printf("Miss\n");
    }

    return 0;
}