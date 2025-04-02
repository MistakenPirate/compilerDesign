%{
#include <stdio.h> 
int flag = 0;      
%}

%%

// If any of these words appear, set flag = 1 (indicating a compound sentence)
and |
or |
but |
because |
if |
then |
nevertheless { flag = 1; }  

. ;  // Ignore all other characters (letters, punctuation, etc.)
\n { return 0; }  // When a newline is encountered, stop scanning

%%

int main() {
    printf("Enter the sentence:\n");
    yylex();
    
    if (flag == 0) {
        printf("Simple sentence\n"); // No conjunctions found -> Simple sentence
    } else {
        printf("Compound sentence\n"); // Conjunction found -> Compound sentence
    }
    
    return 0; // Exit program
}

int yywrap() {
    return 1;
}
