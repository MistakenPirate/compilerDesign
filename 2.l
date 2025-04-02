%{
#include <stdio.h>
%}

%%

[A-Z]+[ \t\n]     { printf("%s is a capital letter\n", yytext); }

// [A-Z]+ correctly matches uppercase words like "HELLO", "WORLD", etc.
// [A_Z]+ also works but behaves differently because:
// - In `[A_Z]`, the underscore `_` is not between `A` and `Z` in ASCII, so it is treated as a separate character.
// - This means `[A_Z]+` matches sequences containing only `A`, `Z`, and `_`, like "A_Z" but not "HELLO".
// - `[A-Z]+` is the correct way to match uppercase words.

. ;

%%

int main() {
    printf("Enter the string:\n");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}
