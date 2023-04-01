#include <assert.h>

#define REGEXP_LIB_IMPLEMENTATION
#include "RegExpLib.h"

void commonSearch() {
    RegExpSearchHit hit;

    char* string = "Mem of the year: LulW";
    char* regexp = ": \\w\\w\\w\\w";

    int hits = RegExp_search(regexp, string, &hit);

    assert(hits == 1 && "commonSearch");
    assert(hit.start == 15 && "commonSearch");
    assert(hit.length == 6 && "commonSearch");
}

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

int main(void) {
    commonSearch();
    staticRegExpWorks();
    dynamicRegExpWorks();

    return 0;
}
