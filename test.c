#include <assert.h>

#define REGEXP_LIB_IMPLEMENTATION
#include "RegExpLib.h"

int main(void) {
    char* string = "Mem of the year: LulW";
    char* regexp = ": \\w\\w\\w\\w";

    int hits = RegExp_search(regexp, string, NULL);

    assert(hits == 1);

    return 0;
}
