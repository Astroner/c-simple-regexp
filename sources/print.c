#include "RegExp.h"

#include <stdio.h>

void RegExp_printSearchHit(const char* string, const RegExpSearchHit* hit) {
    for(size_t i = hit->start; i < hit->start + hit->length; i++) {
        fprintf(stdout, "%c", string[i]);
    }
}

void RegExp_printlnSearchHit(const char* string, const RegExpSearchHit* hit) {
    RegExp_printSearchHit(string, hit);
    fprintf(stdout, "\n");
}

void RegExp_printExpression(const RegExp* regexp) {
    for(size_t i = 0; i < regexp->patternsActualSize; i++) {
        Pattern pattern = regexp->patternsBuffer[i];
        fprintf(stdout, "%zu) ", i + 1);
        switch(pattern.type) {
            case PatternTypeCharacter:
                fprintf(stdout, "%c - Literal", pattern.payload.character);
                break;
            case PatternTypeAnyCharacter:
                fprintf(stdout, ". - Any character");
                break;
            case PatternTypeDigit:
                fprintf(stdout, "\\d - Any digit");
                break;
            case PatternTypeLineStart:
                fprintf(stdout, "^ - Start of the line");
                break;
            case PatternTypeLineEnd:
                fprintf(stdout, "$ - End of the line");
                break;
            case PatternTypeWordCharacter:
                fprintf(stdout, "\\w - Any word character");
                break;
            case PatternTypeSpaceCharacter:
                fprintf(stdout, "\\s - Any white space character");
                break;
        }
        if(pattern.optional) fprintf(stdout, " (Optional)"); 
        if(pattern.transparent) fprintf(stdout, " (Transparent)"); 
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "Minimal Possible Length: %zu\n", regexp->minPossibleLength);
    fprintf(stdout, "Patterns number: %zu; Buffer size: %zu\n", regexp->patternsActualSize, regexp->patternsBufferSize);
}

