#include <stdio.h>

#include "RegExp.h"

int main() {
    RegExpSearchHit hit;

    char* str = "Check this code 2244";
    char* regexp = "code:? \\d\\d\\d\\d";

    int hits = RegExp_search(regexp, str, &hit);

    printf("Hits: %d\n", hits);
    printf("Start: %zu\n", hit.start);
    printf("Length: %zu\n", hit.length);
    printf("Match: '");
    for(size_t i = hit.start; i < hit.start + hit.length; i++) {
        printf("%c", str[i]);
    }
    printf("'\n");

    return 0;
}