#include <assert.h>

#define REGEXP_LIB_IMPLEMENTATION
#include "RegExpLib.h"

int main(void) {
    RegExpSearchHit hit;

    char* string = "Mem of the year: LulW";
    char* regexp = ": \\w\\w\\w\\w";

    int hits = RegExp_search(regexp, string, &hit);

    assert(hits == 1);
    assert(hit.start == 15);
    assert(hit.length == 6);

    return 0;
}
