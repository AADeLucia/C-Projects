/*
ADFGX Cipher
1. Polybius Square 5x5 key square for subsitiution
2. Column transposition with a keyword
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//Convert string to uppercase
void toUpper(char *s) {
    int len = strlen(s);
    int i;
    for(i = 0; i < len; i++) {
        *(s+i) = toupper(*(s+i));
    }
}

//Polybius Square
char pSquare[6][6];
void createSquare() {
    pSquare[0][0] = '\0';
    pSquare[0][1] = 'A';
    pSquare[0][2] = 'D';
    pSquare[0][3] = 'F';
    pSquare[0][4] = 'G';
    pSquare[0][5] = 'X';
    pSquare[1][0] = 'A';
    pSquare[1][1] = 'P';
    pSquare[1][2] = 'H';
    pSquare[1][3] = 'Q';
    pSquare[1][4] = 'G';
    pSquare[1][5] = 'M';
    pSquare[2][0] = 'D';
    pSquare[2][1] = 'E';
    pSquare[2][2] = 'A';
    pSquare[2][3] = 'Y';
    pSquare[2][4] = 'N';
    pSquare[2][5] = 'O';
    pSquare[3][0] = 'F';
    pSquare[3][1] = 'F';
    pSquare[3][2] = 'D';
    pSquare[3][3] = 'X';
    pSquare[3][4] = 'K';
    pSquare[3][5] = 'R';
    pSquare[4][0] = 'G';
    pSquare[4][1] = 'C';
    pSquare[4][2] = 'V';
    pSquare[4][3] = 'S';
    pSquare[4][4] = 'Z';
    pSquare[4][5] = 'W';
    pSquare[5][0] = 'X';
    pSquare[5][1] = 'B';
    pSquare[5][2] = 'U';
    pSquare[5][3] = 'T';
    pSquare[5][4] = 'I';
    pSquare[5][5] = 'L';
}

//Return 2-letter cipher based on 5x5 Polybius Square
char* getCipher(char p) {
    int i, j, len;
    len = 6;
    char* cipher = malloc(sizeof(char)*3);
    for(i = 1; i < len; i++) {
        for(j = 1; j < len; j++) {
            if(pSquare[i][j] == p) {
                cipher[0] = pSquare[0][i];
                cipher[1] = pSquare[0][j];
                cipher[2] = '\0';
            }
        }
    }
    return cipher;
}

//Function for qsort to compare chars
int compareFn(const void *a, const void *b) {
    const char* aa = (const char*)a;
    const char* bb = (const char*)b;
    return strcmp(aa, bb);
}

//Find column order for columnar transposition
//Column order based on aphabetical order of keyword
void colOrder(int* arr, char* s) {
    char* orderedS = malloc(sizeof(s));
    strcpy(orderedS, s);
    qsort(orderedS, strlen(s), sizeof(char), compareFn);

    int i, j;
    for(i = 0; i < strlen(s); i++) {
        for(j = 0; j < strlen(s); j++) {
            if(*(orderedS + i) == *(s + j)) {
                arr[i] = j;
                break;
            }
        }
    }
    
    //If you love memory, set it free!
    free(orderedS);
}

//Columnar transposition
void colTrans(char* keyword, char* ciphInter, char* ciphFinal) {
    
    //Find dimensions of matrix
    int rowLen = strlen(keyword);
    int colLen = strlen(ciphInter) / strlen(keyword);
    
    //Allocate memory for matrix
    char** colTransMatrix = (char **)malloc(sizeof(char *) * rowLen);
    int i;
    for(i = 0; i < rowLen; i++)
        colTransMatrix[i] = (char *)malloc(sizeof(char) * colLen);
    
    //Create matrix from intermediate ciphertext
    //printf("Column Transposition Matrix\n%s\n", keyword);
    int j, pos;
    pos = 0;
    for(i = 0; i < colLen; i++) {
        for(j = 0; j < rowLen; j++) {
            colTransMatrix[i][j] = ciphInter[pos];
            pos++;
            //printf("%c ",  colTransMatrix[i][j]);
        }
        //printf("\n");
    }
    //printf("\n");
    
    //Get column order based on keyword
    int orderOfCols[strlen(keyword)];
    colOrder(orderOfCols, keyword);
    // printf("Column order \n");
    // for(i = 0; i < strlen(keyword); i++) printf("%d ", orderOfCols[i]);
    // printf("\n\n");
    
    //Print columns based on determined order
    pos = 0;
    for(j = 0; j < rowLen; j++) {
        int col = orderOfCols[j];
        for(i = 0; i < colLen; i++) {
            //printf("%c ", colTransMatrix[i][col]);
            *(ciphFinal + pos) = colTransMatrix[i][col];
            //printf("%c ", ciphFinal[pos]);
            pos++;
        }
    }
}

int main(int argc, char* argv[]) {
    
    //Check for correct usage
    if(argc != 2) {
        fprintf(stderr, "Usage: ./adfgx <plaintext>\n");
        exit(1);
    }
    
    //Obtain plaintext
    char* plaintext = argv[1];
    toUpper(plaintext);
    printf("Plaintext: %s\n", plaintext);
    
    //Convert plaintext to ciphertext in 2 steps
    
    //1. Encrypt with Polybius Square
    char* ciphertextInter = malloc(strlen(plaintext)*2 + 1);
    createSquare();
    int i;
    for(i = 0; i < strlen(plaintext); i++)
        strcat(ciphertextInter, getCipher(*(plaintext + i)));
    
    //printf("Intermediate ciphertext: %s\n", ciphertextInter);
    
    //2. Column Transposition with a keyword
    char* keyword = "ECHIDNA";
    char* ciphertext = malloc(strlen(ciphertextInter) + 1);
    colTrans(keyword, ciphertextInter, ciphertext);
    
    printf("\nFinal ciphertext: %s\n", ciphertext);
    
    return 0;
}