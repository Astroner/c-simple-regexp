#include <stdio.h>

#include "RegExp.h"

typedef struct A {
    int a;
} A;

A a[] = {
    { .a = 2, }
};

int main(void) {
    RegExpSearchHit hit;

    char* str = "number: 444a";
    char* regexp = "\\d\\d\\d\\w";

    int hits = RegExp_search(regexp, str, &hit);
    
    printf("HITS: %s\n", hits == SEARCH_HITS ? "TRUE" : hits == SEARCH_EMPTY ? "FALSE" : "SYNTAX_ERROR");
    if(hits) {
        printf("Start: %zu\n", hit.start);
        printf("Length: %zu\n", hit.length);
        printf("Match: '");
        for(size_t i = hit.start; i < hit.start + hit.length; i++) {
            printf("%c", str[i]);
        }
        printf("'\n");
    }

    return 0;
}