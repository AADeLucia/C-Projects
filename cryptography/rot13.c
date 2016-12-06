//Caesar cipher with a rotation of 13 positions

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//Convert string to uppercase
void toUpper(char *s) {
    int len = strlen(s);
    int i;
    for(i = 0; i < len; i++) {
        *(s+i) = toupper(*(s+i));
    }
}

int main(int argc, char *argv[]) {
    
    //Check for correct usage
    if(argc != 2) {
        fprintf(stderr, "Usage: ./rot13 <plaintext>\n");
        exit(1);
    }
    
    //Obtain plaintext
    char* plaintext = argv[1];
    //Ensure plaintext is upper case
    toUpper(plaintext);
    
    //Convert plaintext to ciphertext
    char* ciphertext = malloc(sizeof(plaintext));
    int length = strlen(plaintext);
    int i;
    for(i = 0; i < length; i++) {
        //If more than halfway through alphabet, wrap around
        if(*(plaintext + i) > 'M') {
            *(ciphertext + i) = 'A' + 12 + *(plaintext + i) - 'Z';
        }
        //Else, just increment letter by 13
        else {
            *(ciphertext + i) = *(plaintext + i) + 13;
        }
    }
    
    printf("%s\n%s\n", plaintext, ciphertext);
    
    return 0;
}