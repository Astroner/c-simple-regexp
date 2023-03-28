# Hi there
This is just a simple C RegExp implementation.

# Usage
This repo provide STB like lib *RegExpLib.h*, U can just download it and include to ur project.
Do not forget to define **REGEXP_LIB_IMPLEMENTATION** before **include** statement to get the implementation.

# Supported tokens
 - **.** - any character (except \n, \r etc)
 - **\w** - any word character
 - **\w** - any word character
 - **\s** - any white space character
 - **^** - start of the line
 - **$** - end of the line
 - **\\** - makes any character literal
 - **?** - makes previous token optional

# Functions
## RegExp_search
```c
typedef struct RegExpSearchHit {
    size_t start;
    size_t length;
} RegExpSearchHit; 

int RegExp_search(
    const char* regexp, 
    const char* string, 
    RegExpSearchHit* result
);
```
This function searches for the first substring that matches provided regexp.

Example:
```c
#include <stdio.h>

#define REGEXP_LIB_IMPLEMENTATION
#include "RegExpLib.h"

int main() {
    RegExpSearchHit hit;

    char* str = "Check this code 2244";
    char* regexp = "code:? \\d\\d\\d\\d";

    int hits = RegExp_search(regexp, str, &hit);

    printf("Hits: %d\n", hits);
    printf("Start: %zu\n", hit.start);
    printf("Length: %zu\n", hit.length);
    printf("Match: '");
    for(size_t i = hit.start; i < hit.start + hit.length; i++) {
        printf("%c", str[i]);
    }
    printf("'\n");

    // Output:
    // Hits: 1
    // Start: 11
    // Length: 9
    // Match: 'code 2244'

    return 0;
}
```

# Refs
 - [ASCII codes](https://www.cs.cmu.edu/~pattis/15-1XX/common/handouts/ascii.html)