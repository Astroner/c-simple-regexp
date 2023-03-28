#if !defined(REGEXP_H)
#define REGEXP_H

#include <stdlib.h>

typedef struct RegExpSearchHit {
    size_t start;
    size_t length;
} RegExpSearchHit; 

#define SEARCH_HITS 1
#define SEARCH_EMPTY 0
#define SEARCH_SYNTAX_ERROR -1

int RegExp_search(const char* regexp, const char* str, RegExpSearchHit* result);

#endif // REGEXP_H
#if defined(REGEXP_LIB_IMPLEMENTATION)

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

typedef enum PatternType {
    PatternTypeCharacter,
    PatternTypeAnyCharacter,
    PatternTypeWordCharacter,
    PatternTypeDigit,
    PatternTypeLineStart,
    PatternTypeLineEnd,
    PatternTypeSpaceCharacter,
} PatternType;

typedef struct Pattern {
    PatternType type;
    int optional;
    int transparent;
    union PatternPayload {
        char character;
    } payload;
} Pattern;

size_t identifyPattern(const char* regexp, Pattern* result) {
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
                return SEARCH_SYNTAX_ERROR;
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
                    return SEARCH_HITS;
                }
                return SEARCH_EMPTY;
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
                    if(start > 0 || regexpCursor > 0) return SEARCH_EMPTY;
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
            return SEARCH_HITS;
        }      
    }
    return SEARCH_EMPTY;
}
#endif // REGEXP_LIB_IMPLEMENTATION
