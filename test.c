#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define REGEXP_CHECK_COMPILATION_STATUSES
#define REGEXP_LIB_IMPLEMENTATION
#include "RegExpLib.h"

void staticRegExpWorks() {
    RegExp_createStatic(normal, "...", 3);
    assert(normal->patternsActualSize == 3);
    assert(normal->patternsBufferSize == 3);
    assert(normal->errorStatus == 0);

    RegExp_createStatic(spaceError, "...", 2);
    assert(spaceError->errorStatus == RegExpResultInsufficientSpace && "should throw space issue");

    RegExp_createStatic(syntaxError, "..\\", 3);
    assert(syntaxError->errorStatus == RegExpResultSyntaxError && "should throw syntax error");
}

void dynamicRegExpWorks() {
    RegExp* expr = RegExp_create("...");
    assert(expr->patternsActualSize == 3);
    assert(expr->patternsBufferSize == 3);
    assert(expr->errorStatus == 0);
    RegExp_free(expr);

    expr = RegExp_create("..\\");
    assert(expr->errorStatus == RegExpResultSyntaxError && "should throw syntax error");
    RegExp_free(expr);
}

void staticIsEqualToDynamic() {
    char* str = "2\\d\\d\\d\\s? year";
    RegExp_createStatic(staticExpr, str, 10);

    RegExp* dynamicExpr = RegExp_create(str);

    assert(dynamicExpr->errorStatus == staticExpr->errorStatus && staticExpr->errorStatus == 0);
    assert(dynamicExpr->minPossibleLength == staticExpr->minPossibleLength && staticExpr->minPossibleLength == 9);
    assert(dynamicExpr->patternsActualSize == staticExpr->patternsActualSize && staticExpr->patternsActualSize == 10);
    assert(dynamicExpr->patternsBufferSize == staticExpr->patternsBufferSize && staticExpr->patternsBufferSize == 10);
    assert(memcmp(dynamicExpr->patternsBuffer, staticExpr->patternsBuffer, sizeof(Pattern) * 11));
}

#define COMPILER_TEST(name, regex, mpl, patterns...)\
    do {\
        Pattern name##__expectedPatterns[] = { patterns };\
        Pattern name##__buffer[sizeof(name##__expectedPatterns) / sizeof(Pattern)];\
        RegExp name##__regex = {\
            .patternsBuffer = name##__buffer,\
            .patternsBufferSize = sizeof(name##__expectedPatterns) / sizeof(Pattern),\
        };\
        RegExp_compile(regex, &name##__regex);\
        assert(name##__regex.errorStatus == 0 && #name && regex);\
        assert(name##__regex.minPossibleLength == mpl && #name && regex);\
        assert(name##__regex.patternsActualSize == sizeof(name##__expectedPatterns) / sizeof(Pattern) && #name && regex);\
        for(size_t i = 0; i < sizeof(name##__expectedPatterns) / sizeof(Pattern); i++) {\
            assert(name##__expectedPatterns[i].optional == name##__buffer[i].optional && #name && regex);\
            assert(name##__expectedPatterns[i].transparent == name##__buffer[i].transparent && #name && regex);\
            assert(name##__expectedPatterns[i].type == name##__buffer[i].type && #name && regex);\
            if(name##__expectedPatterns[i].type == PatternTypeCharacter) {\
                assert(name##__expectedPatterns[i].payload.character == name##__buffer[i].payload.character && #name && regex);\
            }\
        }\
    } while(0);\

void compilerTest() {
    COMPILER_TEST(literals, "abc\\.", 4,
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = 'a',
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = 'b',
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = 'c',
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = '.',
        },
    );

    COMPILER_TEST(basicTokens, "..\\d\\d\\w\\w\\s\\s?", 7,
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeAnyCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeAnyCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeDigit,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeDigit,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeWordCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeWordCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeSpaceCharacter,
        },
        {
            .optional = 1,
            .transparent = 0,
            .type = PatternTypeSpaceCharacter,
        }
    )

    COMPILER_TEST(
        mixed,
        "^\\w?\\d\\s 2?.?\\$$",
        4,
        {
            .optional = 0,
            .transparent = 1,
            .type = PatternTypeLineStart,
        },
        {
            .optional = 1,
            .transparent = 0,
            .type = PatternTypeWordCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeDigit,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeSpaceCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = ' ',
        },
        {
            .optional = 1,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = '2',
        },
        {
            .optional = 1,
            .transparent = 0,
            .type = PatternTypeAnyCharacter,
        },
        {
            .optional = 0,
            .transparent = 0,
            .type = PatternTypeCharacter,
            .payload.character = '$',
        },
        {
            .optional = 0,
            .transparent = 1,
            .type = PatternTypeLineEnd,
        },
    )
}

#define SEARCH_TEST(regexp, string, result)\
    do {\
        RegExp* reg = RegExp_create(regexp);\
        RegExpSearchHit hit;\
        int hits = RegExp_search(reg, string, &hit);\
        assert(hits && regexp && string);\
        char* substr = malloc(hit.length);\
        memcpy(substr, (char*)string + hit.start, hit.length);\
        assert(strcmp(substr, result) == 0 && regexp && result);\
        RegExp_free(reg);\
    } while(0);\

void searchTests() {
    SEARCH_TEST("...", "aaa", "aaa");
    SEARCH_TEST("\\d\\d33$", "22334433", "4433");
    SEARCH_TEST("\\d\\d33", "22334433", "2233");
    SEARCH_TEST("obj\\.arr[\\d\\d]", "const a = obj.arr[22]", "obj.arr[22]");
    
    // TODO: Make it work
    // SEARCH_TEST("W\\d?\\d?2$", "W4322W2", "W2");
    // SEARCH_TEST("W\\d?\\d?2$", "W2", "W2");
}

int main(void) {
    staticRegExpWorks();
    dynamicRegExpWorks();
    staticIsEqualToDynamic();
    compilerTest();
    searchTests();

    return 0;
}
