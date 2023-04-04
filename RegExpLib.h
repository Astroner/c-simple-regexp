#if !defined(REGEXP_H)
#define REGEXP_H

#include <stdlib.h>

#if defined(REGEXP_PRINT_COMPILATION_STATUSES)
    #include <stdio.h>
    #define RegExp_createStatic(name, regexp, patternsCount)\
        Pattern name##__buffer[patternsCount];\
        RegExp name##__data = {\
            .patternsBuffer = name##__buffer,\
            .patternsBufferSize = patternsCount,\
        };\
        RegExp* name = &name##__data;\
        RegExpResult name##__status = RegExp_compile(regexp, name);\
        if(name##__status == RegExpResultInsufficientSpace) {\
            fprintf(stderr, "Not enough space for regexp '%s', provided size: %zu\n", regexp, (size_t)patternsCount);\
        } else if(name##__status == RegExpResultSyntaxError) {\
            fprintf(stderr, "Syntax error in regexp '%s'\n", regexp);\
        }
#else
    #define RegExp_createStatic(name, regexp, patternsCount)\
        Pattern name##__buffer[patternsCount];\
        RegExp name##__data = {\
            .patternsBuffer = name##__buffer,\
            .patternsBufferSize = patternsCount,\
        };\
        RegExp* name = &name##__data;\
        RegExp_compile(regexp, name);
#endif


typedef enum RegExpResult {
    RegExpResultHits = 1,
    RegExpResultEmpty = 0,
    RegExpResultSyntaxError = -1,
    RegExpResultInsufficientSpace = -2,
} RegExpResult;

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

typedef struct RegExp {
    RegExpResult errorStatus;
    Pattern* patternsBuffer;
    size_t patternsBufferSize;
    size_t patternsActualSize;
    size_t minPossibleLength;
} RegExp;

typedef struct RegExpSearchHit {
    size_t start;
    size_t length;
} RegExpSearchHit; 

RegExpResult RegExp_compile(const char* regexp, RegExp* result);
RegExpResult RegExp_search(const RegExp* regexp, const char* str, RegExpSearchHit* result);
void RegExp_printSearchHit(const char* string, const RegExpSearchHit* hit);
void RegExp_printlnSearchHit(const char* string, const RegExpSearchHit* hit);

RegExp* RegExp_create(const char* regexp);
void RegExp_free(RegExp*);

void RegExp_printExpression(const RegExp* regexp);

#endif // REGEXP_H
#if defined(REGEXP_LIB_IMPLEMENTATION)

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

static size_t getPatternsNumber(const char* regexp) {
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
    size_t minPossibleLength = 0;

    size_t patternIndex = 0;
    while(regexp[cursor]) {
        if(patternIndex == result->patternsBufferSize) {
            result->errorStatus = RegExpResultInsufficientSpace;
            return RegExpResultInsufficientSpace;
        }

        Pattern* pattern = result->patternsBuffer + patternIndex;
        size_t patternLength = identifyPattern(regexp + cursor, pattern);

        if(!patternLength) {
            result->errorStatus = RegExpResultSyntaxError;
            return RegExpResultSyntaxError;
        }
        if(!pattern->optional && !pattern->transparent) {
            minPossibleLength++;
        }
        patternIndex++;
        cursor += patternLength;
    }

    result->patternsActualSize = patternIndex;
    result->errorStatus = 0;
    result->minPossibleLength = minPossibleLength;

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

RegExpResult RegExp_search(const RegExp* regexp, const char* str, RegExpSearchHit* result) {
    // Check for errors in RegExp
    if(regexp->errorStatus < 0) return regexp->errorStatus;

    size_t start = 0;
    // we are iterating the string while it is possible for the substring to fit into the space left
    // we handle the scenario where string length is less then minPossibleLength after regexp loop
    // We cant handle it here without getting length of the string
    while(str[start + regexp->minPossibleLength - 1]) {
        int hits = 1;

        size_t regexpCursor = 0; // regexp pattern index
        size_t stringCursor = 0; // string char index. Should be separated from regexpCursor, coz 1 pattern != 1 char.
        while(regexpCursor < regexp->patternsActualSize) {
            Pattern pattern = regexp->patternsBuffer[regexpCursor];
            char ch = str[start + stringCursor];

            int isLastPattern = regexpCursor + 1 == regexp->patternsActualSize;
            int isEndOfTheString = !ch;

            if(
                // if it is the last pattern and it is optional then we dont need to even handle it, it is auto match
                (isLastPattern && pattern.optional)
                ||
                // also it is better to handle End of the string at this place because after it's condition we can just break current loop
                // and proceed directly to results
                (pattern.type == PatternTypeLineEnd && isEndOfTheString && isLastPattern)
            ) break;

            int matches = 0;

            switch(pattern.type) {
                case PatternTypeAnyCharacter:
                    matches = ch != CharCodeNL && ch != CharCodeCR;
                    break;

                case PatternTypeCharacter:
                    matches = ch == pattern.payload.character;
                    break;

                case PatternTypeDigit:
                    matches = ch >= CharCode0 && ch <=CharCode9;
                    break;

                case PatternTypeWordCharacter:
                    matches = (
                        (ch >= CharCodeA && ch <= CharCodeZ)
                        || 
                        (ch >= CharCodeCapitalA && ch <= CharCodeCapitalZ)
                    );
                    break;

                case PatternTypeSpaceCharacter:
                    matches = ch == CharCodeSpace || ch == CharCodeTab;
                    break;

                case PatternTypeLineEnd:
                    matches = 0;
                    break;

                case PatternTypeLineStart:
                    if(start > 0 || regexpCursor > 0) return RegExpResultEmpty;
                    matches = 1;
                    break;
            }
            
            if(matches) { // if pattern matches char
                // then we go to the next pattern
                regexpCursor++;
                
                // end move to the next char unless pattern is transparent(^ for example) and it is end of the string.
                if(!pattern.transparent && !isEndOfTheString) {
                    stringCursor++;
                }
            } else { // if it doesn't match
                if(pattern.optional) { // if pattern is optional (?)
                    regexpCursor++; // then we just move to the next pattern and dont move stringCursor
                } else { // if it is not optional we need to move to the next substring
                    hits = 0;
                    break;
                }
            }

            // if we are at the end of the string and current pattern is not the last one
            // then we need to return Empty except it is optional pattern
            // if it is an optional pattern then we need to continue to handle multiple optional patterns at the end of the RegExp. "a?a?$" or "a?a?"
            if(isEndOfTheString && !isLastPattern && !pattern.optional) {
                return RegExpResultEmpty;
            }
        }
        
        // here we need to handle "str length < minPossibleLength" scenario
        // If we met Nul Terminator during the iteration and start == 0 then stringCursor will be equal to length of the string
        if(start == 0 && !str[stringCursor] && stringCursor < regexp->minPossibleLength) {
            return RegExpResultEmpty;
        }
    
        if(hits) {
            if(result) {
                result->start = start;
                result->length = stringCursor;
            }
            return RegExpResultHits;
        } else {
            start++;
        }
    }

    return RegExpResultEmpty;
}

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

#endif // REGEXP_LIB_IMPLEMENTATION
