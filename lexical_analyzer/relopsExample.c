// A lexical analyzer for relational operators
// DSM, 2016

// The program recognizes the six basic Pascal-style relational operators
// =, <, >, <=, >=, <>

#include <stdio.h>
#include <stdlib.h>


/*** Analyze a file ***/

// Inputs:
//  char *filename : the name of the input file

// Outputs:
//  none: tokens are generated as the analysis executes

void analyze(char *filename) {
    
    // Open the file and check for errors
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("open");
        exit(1);
    }
    
    // Loop until reaching EOF
    while (1) {
        
        // Read the next character
        char c = (char) fgetc(f);
        
        // Check for end-of-file
        if (c == EOF) {
            break;
        }
        
        // Check for errors
        else if (c < 0) {
            perror("read");
            exit(1);
        }
        
        // If the token is =, it can only be an equals operator
        else if (c == '=') {
            printf("Token: <EQUAL>\n");
        }
        
        // If the token is <, there are three possible cases
        else if (c == '<') {
            
            // Read the next character to determine which case applies
            c = (char) fgetc(f);
            
            // <= case
            if (c == '=') {
                printf("Token: <NOTGREATER>\n");
            }
            
            // <> case
            else if (c == '>') {
                printf("Token: <NOT_EQUAL>\n");
            }
            
            // Default case: <
            else {
                printf("Token: <LESS>\n");
                
                // The current value of c is not part of this token.  Put it 
                // back on the stream so it can be read as the start of a new
                // token
                ungetc(c, f);
            }
        }
        
        // Similar cases for >
        else if (c == '>') {
            c = (char) fgetc(f);

            // >= case
            if (c == '=') {
                printf("Token: <NOTLESS>\n");
            }
            
            // Default case is >
            else {
                printf("Token: <GREATER>\n");
                
                // Push the current character back on the stream
                ungetc(c, f);
            }
        }
    }
    
    return;
}

int main() {
    
    analyze("relops_test.txt");
    return 0;
}