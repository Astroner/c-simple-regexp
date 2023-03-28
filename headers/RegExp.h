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
