// Combination of fork(), exec(), and wait()
// DSM, 2016

// Use wait to make the parent process suspend execution until its child
// finishes. This can be used to make an interactive shell.

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    int rc = fork();

    // Child runs execvp
    if (rc == 0) {
        
        // Call sleep to make the process nap for 10 seconds
        char *args[3];
        args[0] = "sleep";
        args[1] = "10";
        args[2] = NULL;
        
        execvp(args[0], args);
        
        // We should never reach here
        perror("execvp");
        return -1;
    }
    
    // Parent waits for child to finish
    else if (rc > 0) {
        printf("Parent will wait for child to terminate...\n");
        
        // The basic wait system call suspends the calling process until one
        // of its children terminates
        int rc = wait(NULL);
        if (rc < 0) {
            perror("wait");
            return -1;
        }
        
        printf("Parent is done waiting!\n");
    }
    
    else {
        perror("fork");
    }
    
    return 0;
}