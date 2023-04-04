# Hi there
This is just a simple C RegExp implementation.

# Table of content
 - [Include to your project](#include-to-your-project)
 - [Supported tokens](#supported-tokens)
 - [Usage](#usage)
     - [Structs](#structs)
     - [Create RegExp](#create-regexp)
         - [Static with macro](#static-with-macro)
         - [Dynamic with function](#dynamic-with-function)
 - [Refs](#refs)

# Include to your project
This repo provide STB like lib *RegExp.h*, U can just download it and include into ur project.
Do not forget to define **REGEXP_IMPLEMENTATION** before **include** statement to get the implementation.

# Supported tokens
 - **.** - any character (except \n, \r etc)
 - **\w** - any word character
 - **\d** - any digit
 - **\s** - any white space character
 - **^** - start of the line
 - **$** - end of the line
 - **\\** - makes any character literal
 - **?** - makes previous token optional

# Usage
## Structs
This lib provides 3 basic structures:
 - **RegExp** - regular expression. Useful fields:
     - **errorStatus** - **RegExpResult** - provides error information during the compilation.
 - **RegExpResult** - enum containing various results:
     - **RegExpResultEmpty** - nothing is found
     - **RegExpResultHits** - found something
     - **RegExpResultSyntaxError** - syntax error during regexp compilation
     - **RegExpResultInsufficientSpace** - not enough space during static compilation
 - **RegExpSearchHit** - represents search hit. Contains 2 fields:
     - **start** - start of the substring
     - **length** - length of the substring

## Create RegExp
At first you need to create RegExp and compile it. Basically, you have 3 ways to do so:
### Static with macro
*RegExp.h* provides **RegExp_createStatic(name, regexp, patternsNumber)**.
This macro will create pointer to **RegExp** with provided **name** in current scope. 
Args:
 - **name** - RegExp name
 - **regexp** - regexp string
 - **patternsNumber** - buffer size for the regexp

If you want to get compilation errors into the console, you need to define **REGEXP_PRINT_COMPILATION_STATUSES** before **include** statement
or you can check them by yourself with **errorStatus** field.

Example:
```c
#define REGEXP_IMPLEMENTATION
#define REGEXP_PRINT_COMPILATION_STATUSES
#include "RegExp.h"

int main(void) {
    RegExp_createStatic(expression, "n.mb\\wr$", 7);
    if(expression->errorStatus < 0) {
        printf("Got error during the compilation: ");
        if(expression->errorStatus == RegExpResultInsufficientSpace) {
            printf("Insufficient Buffer Space\n");
        } else if(expression->errorStatus == RegExpResultSyntaxError) {
            printf("Syntax Error\n");
        }
        return -1;
    }

    return 0;
}
```
### Dynamic with function
This lib provide **RegExp* RegExp_create(const char* regexp)** function to dynamically create regexp.
To free created regexp you can use **void RegExp_free(RegExp*)** function.
If you want to get compilation errors into the console, you need to define **REGEXP_PRINT_COMPILATION_STATUSES** before **include** statement
or you can check them by yourself with **errorStatus** field.

Example:
```c
#define REGEXP_IMPLEMENTATION
#define REGEXP_PRINT_COMPILATION_STATUSES
#include "RegExp.h"

int main(void) {
    RegExp* expression = RegExp_create("n.mb\\wr$");

    if(!expression) {
        printf("Failed to create regexp\n");
        return -1;
    }
    if(expression->errorStatus < 0) {
        printf("Got error during the compilation: ");
        if(expression->errorStatus == RegExpResultSyntaxError) {
            printf("Syntax Error\n");
        }

        RegExp_free(expression);
        return -1;
    }
    // ...code

    RegExp_free(expression);

    return 0;
}
```
# Refs
 - [ASCII codes](https://www.cs.cmu.edu/~pattis/15-1XX/common/handouts/ascii.html)
