/*Project 3: Lexical Analyzer*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Check if identifier is a keyword
char* keywords[] = {"array", "begin", "const", "do", "else", "end", "if", "of", "procedure", "program", "record", "then", "type", "var", "while"};
int isKeyword(char* c) {
    int i;
    for(i = 0; i < 15; i++) {
        if(strcasecmp(c, keywords[i]) == 0) return 1;
    }
    return 0;
}

//Analyze identifiers
void analyzeIdentifier(FILE *f) {
    //Allocate space for identifier
    int initSpace = 128 * sizeof(char);
    char *ident = (char *) malloc(initSpace);
    
    while(1) {
        char c = (char) fgetc(f);
        
        //End if character is not part of name
        if(!isalpha(c) && !isdigit(c)) {
            
            //Check if keyword
            if(isKeyword(ident)) printf("<%s>", ident);
            
            //Else ID
            else printf("<NAME, %s>", ident);
            
            //Pushback
            ungetc(c, f);
            
            break;
        }
        
        //Else add char to identifier
        else {
            //Check for enough memory
            if(initSpace >= sizeof(ident)) strncat(ident, &c, 1);
            
            //Else allocate more
            else ident = realloc(ident, 256);
        }
    }
    
    return;
}

//Analyze numerals
void analyzeNumber(FILE *f) {
    //Allocate space for numeral
    int initSpace = 128 * sizeof(char);
    char *num = (char *) malloc(initSpace);
    
    while(1) {
        char c = (char) fgetc(f);
        
        //End if character is not part of name
        if(!isdigit(c)) {
            
            //Print number
            printf("<NUMERAL, %s>", num);
            
            //Pushback
            ungetc(c, f);
            
            break;
        }
        
        //Else add char to number
        else {
            //Check for enough memory
            if(initSpace >= sizeof(num)) strncat(num, &c, 1);
            
            //Else allocate more
            else num = realloc(num, 256);
        }
    }
    
    return;
}

//Check if char is whitespace
int isWhitespace(char c) {
    if(c == ' ' || c == '\t') return 1;
    else return 0;
}

//Analyze comments
void analyzeComment(FILE *f) {
    
    while(1) {
        char c = (char) fgetc(f);
        
        //Check EOF
        if(c == EOF) {
            fprintf(stderr, "Reached end of file during comment\n");
            exit(1);
        }
        
        //Check for end of token
        else if(c == '}') {
            printf("<COMMENT>");
            break;
        }
    }
    
    return;
}

//Handle UNKNOWN Token
void errorUnknown(char c, int line) {
    fprintf(stderr, "Unknown token '%c' on line %d\n", c, line);
    exit(1);
}

void analyze(char* filename) {
    
    //Open file and check for errors
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        fprintf(stderr, "File not found: %s\n", filename);
        exit(1);
    }
    
    //Initialize state variable
    int state = 0;
    
    //Initialize line number
    int lineNumber = 1;
    
    //Loop until EOF
    while(1) {
        char c = (char) fgetc(f);
  
        switch(state) {
            //Starting point
            case 0:
                //State: Whitespace
                if(isWhitespace(c)) state = 0;
                
                //State: Digit
                else if(isdigit(c)) {
                    //Push char back onto stream
                    ungetc(c, f);
                    analyzeNumber(f);
                }
                
                //State: Alphabetic
                else if (isalpha(c)) {
                    //Push char back onto stream
                    ungetc(c, f);
                    analyzeIdentifier(f);
                }
                
                //State: Comment
                else if(c == '{') analyzeComment(f);
                
                //Final State: ASTERICK
                else if(c == '*') printf("<ASTERICK>");
                
                //Final State: COMMA
                else if(c == ',') printf("<COMMA>");
                
                //Final State: DIV
                else if(c == '/') printf("<DIV>");
                
                //Final State: EQUALS
                else if(c == '=') printf("<EQUALS>");
                
                //Final State: LEFTBRACKET
                else if(c == '[') printf("<LEFTBRACKET>");
                
                //Final State: LEFTPARENS
                else if(c == '(') printf("<LEFTPARENS>");
                
                //Final State: MINUS
                else if(c == '-') printf("<MINUS>");
                
                //Final State: MOD
                else if(c == '%') printf("<MOD>");
                
                //Final State: NOT
                else if(c == '!') printf("<NOT>");
                
                //Final State: PLUS
                else if(c == '+') printf("<PLUS>");
                
                //Final State: RIGHTBRACKET
                else if(c == ']') printf("<RIGHTBRACKET>");
                
                //Final State: RIGHTPARENS
                else if(c == ')') printf("<RIGHTPARENS>");
                
                //Final State: SEMICOLON
                else if(c == ';') printf("<SEMICOLON>");
                
                //State 1: &
                else if(c == '&') state = 1;
                
                //State 2: :
                else if(c == ':') state = 2;

                //State 3: .
                else if(c == '.') state = 3;
                
                //State 4: >
                else if(c == '>') state = 4;
                
                //State 5: <
                else if(c == '<') state = 5;
                
                //State 6: |
                else if(c == '|') state = 6;
                
                //Line counter
                else if(c == '\n') {
                    lineNumber++;
                    printf("\n");
                }
                
                //UNKNOWN
                else if(c != EOF) errorUnknown(c, lineNumber);
                
                break;
                
            //State 1: &
            case 1:
                //Final State: AND
                if(c == '&')  printf("<AND>");
                
                //UNKNOWN
                else errorUnknown(c, lineNumber);
                
                state = 0;
                
                break;
            
            //State 2: :
            case 2:
                //Final State: BECOMES
                if(c == '=') printf("<BECOMES>");
                
                //Final State: COLON
                else {
                    printf("<COLON>");
                    ungetc(c, f);
                }
                
                state = 0;
                
                break;
            
            //State 3: .
            case 3:
                //Final State: DOUBLDOT
                if(c == '.') printf("<DOUBLEDOT>");
                
                //Final State: PERIOD
                else {
                    printf("<PERIOD>");
                    ungetc(c, f);
                }
                
                state = 0;
                
                break;
            
            //State 4: >
            case 4:
                //Final State: NOTLESS
                if(c == '=') printf("<NOTLESS>");
                
                //Final State: GREATER
                else {
                    printf("<GREATER>");
                    ungetc(c, f);
                }
                
                state = 0;
                
                break;
            
            //State 5: <
            case 5:
                //Final State: NOTGREATER
                if(c == '=') printf("<NOTGREATER>");
                
                //Final State: NOTEQUAL
                else if(c == '>') printf("<NOTEQUAL>");
                
                //Final State: LESS
                else {
                    printf("<LESS>");
                    ungetc(c, f);
                }
                
                state = 0;
                
                break;
            
            //State 6: |
            case 6:
                //Final State: OR
                if(c == '|') printf("<OR>");
                
                //UNKNOWN
                else errorUnknown(c, lineNumber);
                
                //Return to base state
                state = 0;
                
                break;
        }
    
        //Check for EOF
        if(c == EOF) {
            printf("ENDFILE\n");
            break;
        }
                
        //Check for error    
        else if (c < 0) {
            fprintf(stderr, "Error while reading.\n");
            exit(1);
        }
    }
    
    return;
}


int main(int argc, char* argv[]) {
    
    //Check for correct usage of program
    if(argc != 3) {
        printf("Usage: ./lexer -i <Pascal input file>\n");
        exit(1);
    }
    
    char* input = argv[2];
    
    //Analyze file
    analyze(input);
    
    return 0;
}
