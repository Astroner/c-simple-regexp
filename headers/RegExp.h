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
RegExpResult RegExp_searchRaw(const char* regexp, const char* str, RegExpSearchHit* result);
RegExpResult RegExp_search(const RegExp* regexp, const char* str, RegExpSearchHit* result);
void RegExp_printSearchHit(const char* string, const RegExpSearchHit* hit);
void RegExp_printlnSearchHit(const char* string, const RegExpSearchHit* hit);

RegExp* RegExp_create(const char* regexp);
void RegExp_free(RegExp*);

void RegExp_printExpression(const RegExp* regexp);

#endif // REGEXP_H
