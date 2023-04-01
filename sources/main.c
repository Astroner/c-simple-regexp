#include <stdio.h>

#define REGEXP_PRINT_COMPILATION_STATUSES
#include "RegExp.h"

int main() {
    RegExp* match = RegExp_create("aasd...");
    RegExp_free(match);

    return 0;
}