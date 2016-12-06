// A lexical analyzer for relational operators and identifiers
// DSM, 2016

// The program recognizes the six basic Pascal-style relational operators
// =, <, >, <=, >=, <>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for isalpha() and isdigit()


/*** Analyze an identifier ***/

// Inputs:
//  FILE *f: the file

// Outputs:
//  none: tokens are identified as the program runs
void analyzeIdentifier(FILE *f) {
    
    // Allocate a buffer to hold the identifier
    char *s = (char *) malloc(128 * sizeof(char));
    
    while (1) {
        // Read the next character
        char c = (char) fgetc(f);
        
        // If the character is not alphabetic, we're done
        if (!isalpha(c)) {
            
            // Check here if the token is a reserved keyword
            
            // If it's not, report it as an ID
            printf("Token: <NAME, %s>\n", s);
            
            // Push the character back on the stream
            ungetc(c, f);
            
            break;
        }
        
        // Else, add it to the identifier
        else {
            // Use strncat to make sure that only one character is appended
            strncat(s, &c, 1);
            
            // Maybe we should check to make sure the buffer isn't full?
        }
    }
    
    return;
}

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
        
        // If the character is alphabetic, look for an identifier
        else if (isalpha(c)) {
            
            // Push the current character back on the stream
            ungetc(c, f);
            
            analyzeIdentifier(f);
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
                printf("Token: <NOTEQUAL>\n");
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
    
    analyze("relops_and_ids_test.txt");
    return 0;
}