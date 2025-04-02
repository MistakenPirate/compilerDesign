%{
#include <stdio.h>
int v = 0, c = 0;
%}

%%

[ \t]+         ; // Ignore spaces and tabs
\n             { return 0; } // Stop scanning on newline
[aeiouAEIOU]   { v++; }  // Count vowels
[a-zA-Z]       { c++; }  // Count consonants
.              ;    // Ignore other characters     

%%

int main() {
    printf("Enter the string:\n");
    yylex();
    printf("No. of vowels: %d\n", v);
    printf("No. of consonants: %d\n", c);
    return 0;
}

int yywrap() {
    return 1;
}
