#include <stdio.h>

#define REGEXP_PRINT_COMPILATION_STATUSES
#include "RegExp.h"

int main() {
    char* str = "33";
    RegExp* expr = RegExp_create("22222");
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