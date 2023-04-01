#include "RegExp.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum CharCode {
    CharCodeSpace = 32,
    CharCodeCapitalA = 65,
    CharCodeCapitalZ = 90,
    CharCodeA = 97,
    CharCodeZ = 122,
    CharCodeDot = 46,
    CharCode0 = 48,
    CharCode9 = 57,
    CharCodeNL = 10,
    CharCodeCR = 13,
    CharCodeBackSlash = 92,
    CharCodeW = 119,
    CharCodeD = 100,
    CharCode$ = 36,
    CharCodeCaret = 94, //^
    CharCodeSlash = 47,
    CharCodeQuestionMark = 63,
    CharCodeTab = 9,
    CharCodeS = 115,
} CharCode;

static size_t identifyPattern(const char* regexp, Pattern* result) {
    result->optional = 0;
    result->transparent = 0;
    result->type = 0;

    size_t length = 0;
    switch(regexp[0]) {
        case CharCodeBackSlash:
            if(!regexp[1]) return 0;
            switch(regexp[1]) {
                case CharCodeD:
                    result->type = PatternTypeDigit;
                    break;

                case CharCodeW:
                    result->type = PatternTypeWordCharacter;
                    break;

                case CharCodeS:
                    result->type = PatternTypeSpaceCharacter;
                    break;

                default:
                    result->type = PatternTypeCharacter;
                    result->payload.character = regexp[1];
            }
            length = 2;
            break;
        
        case CharCodeDot:
            result->type = PatternTypeAnyCharacter;
            length = 1;
            break;

        case CharCodeCaret:
            result->type = PatternTypeLineStart;
            result->transparent = 1;
            length = 1;
            break;

        case CharCode$:
            result->type = PatternTypeLineEnd;
            result->transparent = 1;
            length = 1;
            break;

        default:
            result->type = PatternTypeCharacter;
            result->payload.character = regexp[0];
            length = 1;
    }

    if(regexp[length] == CharCodeQuestionMark) {
        length += 1;
        result->optional = 1;
    }

    return length;
}

size_t getPatternsNumber(const char* regexp) {
    size_t cursor = 0;

    size_t patternIndex = 0;
    Pattern pattern;
    Pattern* patternPtr = &pattern;
    while(regexp[cursor]) {
        size_t patternLength = identifyPattern(regexp + cursor, patternPtr);

        if(!patternLength) {
            return 0;
        }
        patternIndex++;
        cursor += patternLength;
    }

    return patternIndex;
}

RegExpResult RegExp_compile(const char* regexp, RegExp* result) {
    size_t cursor = 0;

    size_t patternIndex = 0;
    while(regexp[cursor]) {
        if(patternIndex == result->patternsBufferSize) {
            result->errorStatus = RegExpResultInsufficientSpace;
            return RegExpResultInsufficientSpace;
        }

        size_t patternLength = identifyPattern(regexp + cursor, result->patternsBuffer + patternIndex);

        if(!patternLength) {
            result->errorStatus = RegExpResultSyntaxError;
            return RegExpResultSyntaxError;
        }
        patternIndex++;
        cursor += patternLength;
    }

    result->patternsActualSize = patternIndex;
    result->errorStatus = 0;

    return RegExpResultHits;
}

RegExp* RegExp_create(const char* regexp) {
    size_t size = getPatternsNumber(regexp);

    if(!size) {
        #if defined(REGEXP_PRINT_COMPILATION_STATUSES) 
            fprintf(stderr, "Syntax error in '%s'\n", regexp);
        #endif

        RegExp* result = malloc(sizeof(RegExp));
        result->errorStatus = RegExpResultSyntaxError;
        result->patternsBufferSize = 0;
        result->patternsActualSize = 0;

        return result;
    }

    char* memory = malloc(sizeof(RegExp) + sizeof(Pattern) * size);

    if(!memory) {
        #if defined(REGEXP_PRINT_COMPILATION_STATUSES) 
            fprintf(stderr, "Failed to allocate memory for regexp '%s'\n", regexp);
        #endif
        return NULL;
    }

    RegExp* result = (void*)memory;
    Pattern* buffer = (Pattern*)(memory + sizeof(RegExp));

    result->patternsBuffer = buffer;
    result->patternsBufferSize = size;

    RegExp_compile(regexp, result);

    return result;
}

void RegExp_free(RegExp* regexp) {
    free(regexp);
}

int RegExp_search(const char* regexp, const char* str, RegExpSearchHit* result) {

    size_t start = 0;
    while(str[start]) {

        size_t regexpCursor = 0;
        size_t stringCursor = 0;

        int hits = 1;

        while(regexp[regexpCursor]) {
            Pattern pattern;
            size_t patternLength = identifyPattern(regexp + regexpCursor, &pattern);

            if(!patternLength) {
                return RegExpResultSyntaxError;
            }
        
            char stringChar = str[start + stringCursor];
            if(!stringChar) {
                if(
                    !regexp[regexpCursor + patternLength] // last regexp pattern
                    &&
                    (pattern.optional || pattern.type == PatternTypeLineEnd)
                ) {
                    if(result) {
                        result->start = start;
                        result->length = stringCursor;
                    }
                    return RegExpResultHits;
                }
                return RegExpResultEmpty;
            }

            int matches = 0;
            switch(pattern.type) {
                case PatternTypeCharacter:
                    matches = stringChar == pattern.payload.character;
                    break;

                case PatternTypeAnyCharacter:
                    matches = stringChar != CharCodeNL && stringChar != CharCodeCR;
                    break;

                case PatternTypeDigit:
                    matches = (stringChar >= CharCode0) && (stringChar <= CharCode9);
                    break;
                
                case PatternTypeWordCharacter:
                    matches = (
                        (stringChar >= CharCodeCapitalA && stringChar <= CharCodeCapitalZ)
                        ||
                        (stringChar >= CharCodeA && stringChar <= CharCodeZ)
                    );
                    break;

                case PatternTypeSpaceCharacter:
                    matches = (
                        stringChar == CharCodeNL
                        || stringChar == CharCodeSpace 
                        || stringChar == CharCodeTab
                    );
                    break;

                case PatternTypeLineStart:
                    if(start > 0 || regexpCursor > 0) return RegExpResultEmpty;
                    matches = 1;
                    break;

                case PatternTypeLineEnd:
                    matches = !str[start + stringCursor + 1];
                    break;
            }

            regexpCursor += patternLength;
            if(matches) {
                if(!pattern.transparent) {
                    stringCursor++;
                }
            } else if(!pattern.optional) {
                hits = 0;
                break;
            }
        }

        if(!hits) {
            start++;
        } else {
            if(result) {
                result->start = start;
                result->length = stringCursor;
            }
            return RegExpResultHits;
        }      
    }
    return RegExpResultEmpty;
}

void RegExp_printExpression(const RegExp* regexp) {
    for(size_t i = 0; i < regexp->patternsActualSize; i++) {
        Pattern pattern = regexp->patternsBuffer[i];
        fprintf(stdout, "%zu) ", i + 1);
        switch(pattern.type) {
            case PatternTypeCharacter:
                fprintf(stdout, "%c - Literal\n", pattern.payload.character);
                break;
            case PatternTypeAnyCharacter:
                fprintf(stdout, ". - Any character\n");
                break;
            case PatternTypeDigit:
                fprintf(stdout, "\\d - Any digit\n");
                break;
            case PatternTypeLineStart:
                fprintf(stdout, "^ - Start of the line\n");
                break;
            case PatternTypeLineEnd:
                fprintf(stdout, "$ - End of the line\n");
                break;
            case PatternTypeWordCharacter:
                fprintf(stdout, "\\w - Any word character\n");
                break;
            case PatternTypeSpaceCharacter:
                fprintf(stdout, "\\s - Any white space character\n");
                break;
        }
    }
    fprintf(stdout, "Patterns number: %zu; Buffer size: %zu\n", regexp->patternsActualSize, regexp->patternsBufferSize);
}

