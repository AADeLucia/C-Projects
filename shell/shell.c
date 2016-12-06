/****Project 4****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS_SIZE 10
#define MAX_COMMAND_SIZE 60

//Search array for specified value
int search(char* args[], char* c, char* file) {
    int i = 0;
    
    while(args[i] != NULL) {
        
        if(strcmp(args[i], c) == 0) {
            
            //If redirect
            if(strcmp(c, ">") == 0) {
                //Save output file name
                strcpy(file, args[++i]);
                //Clear out excess arguments
                args[i] = NULL;
                args[--i] = NULL;
            }
            
            //If background process
            else if(strcmp(c, "&") == 0) {
                //Clear out excess arguments
                args[i] = NULL;
            }
            return 1;
        }
        i++;
    }
    return 0;
}

//Clean contents from array
void clean(char* args[]) {
   
    int i;
    for(i = 0; i < MAX_ARGS_SIZE; i++) {
        args[i] = NULL;
    }
}

//Tokenize string input
void tokenizeCommand(char* command, char* args[]) {
    
    //Erase last command from args
    clean(args);
    
    //Create copy of string to tokenize
    char* b = malloc(sizeof(command));
    strcpy(b, command);
    
    int i = 0;
    char* token = strtok(b, " \n");
    while(token) {
        //Put tokens into array
        args[i] = token;
        
        i++;
        token = strtok(NULL, " \n");
    }
    
}

int main() {
    
    //Set flags
    int true = 1;
    int isRedirect = 0;
    int isBackground = 0;
    
    //Allocate memory for vars
    char* command = malloc(MAX_COMMAND_SIZE * sizeof(char));
    char* args[MAX_ARGS_SIZE];
    char file[30];

    //Loop shell
    while(true) {
        
        //Prompt user
        printf("shell $ ");
        fgets(command, MAX_COMMAND_SIZE, stdin);

        //Check that a command is entered
        if(strcmp(command, "\n") == 0) {
            continue;
        }
        
        //Tokenize command
        tokenizeCommand(command, args);
        
        //Check if redirect or background process
        //Set flags appropriately
        if(search(args, ">", file)) isRedirect = 1;
        if(search(args, "&", file)) isBackground = 1;
        
        //Command: Exit shell
        if(strcmp(args[0], "exit") == 0) {
            printf("exiting shell...\n");
            true = 0;
        }
        
        //Command: Change directory
        else if(strcmp(args[0], "cd") == 0) {
            chdir(args[1]);
        }
        
        //Other commands
        else {
            //Create child process
            int rc = fork();
            
            //Send command to child
            if(rc == 0) {
                //Redirect output if flagged
                if(isRedirect) {
                    //Close stdout to free a file descriptor
                    close(1);
                    //File now takes fd 1, the lowest available descriptor
                    open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                }
                
                //Execute command
                execvp(args[0], args);
                
                perror("execvp");
                return -1;
            }
            
            //Parent waits until child is finished
            else if(rc > 0) {
                //Do not wait if it flagged as not a background process
                if(!isBackground) {
                    int rc = wait(NULL);
                    if(rc < 0) {
                        perror("wait");
                        return -1;
                    }
                }
            }
            
            //Check for errors
            else {
                perror("fork");
                return -1;
            }
        }
        
        //Reset flags
        isBackground = 0;
        isRedirect = 0;
    }
    
    //If you love memory, set it free
    free(command);
    return 0;
}