#include <stdio.h>
#include <stdlib.h>

#define REGEXP_IMPLEMENTATION
#include "RegExp.h"

int main(void) {
    char* regexp = "..dot..";
    size_t patternsNumber = RegExp_patternsNumber(regexp);
    if(patternsNumber == 0) {
        printf("Got syntax error\n");
        return 1;
    }
    Pattern* buffer = malloc(sizeof(Pattern) * patternsNumber);
    RegExp expression = {
        .patternsBuffer = buffer,
        .patternsBufferSize = patternsNumber,
    };

    RegExpResult result = RegExp_compile("word", &expression);

    free(buffer);

    return 0;
}