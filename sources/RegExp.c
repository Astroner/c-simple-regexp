#include "RegExp.h"

#include <stdio.h>

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
} CharCode;

int RegExp_search(const char* regexp, const char* str, RegExpSearchHit* result) {

    size_t start = 0;
    while(str[start]) {
        size_t regIndex = 0;

        int isEscaped = 0;
        char strChar;
        char regChar;
        size_t length = 0;
        while((regChar = regexp[regIndex])) {
            strChar = str[start + length];
            if(!strChar) {
                return SEARCH_EMPTY;
            }
            switch(regChar) {
                case CharCodeDot:
                    if(strChar == CharCodeCR || strChar == CharCodeSpace || strChar == CharCodeNL)
                        goto move_start;
                    break;
                case CharCodeBackSlash:
                    isEscaped = 1;
                    regIndex++;
                    continue;
                case CharCodeD:
                    if(isEscaped) {
                        if(strChar < CharCode0 || strChar > CharCode9) goto move_start;
                    } else {
                        goto default_handler;
                    }
                    break;
                case CharCodeW:
                    if(isEscaped) {
                        if(
                            (strChar < CharCodeCapitalA || strChar > CharCodeCapitalZ) 
                            && 
                            (strChar < CharCodeA || strChar > CharCodeZ)
                        ) goto move_start;
                    } else {
                        goto default_handler;
                    }
                    break;
                default:
default_handler:
                    if(strChar != regChar) goto move_start;
            }
            isEscaped = 0;
            regIndex++;
            length++;
        }
        result->start = start;
        result->length = length;
        return SEARCH_HITS;
move_start:
        start++;
    }

    return SEARCH_EMPTY;
}