#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILD 5

void sigint_handler(int a) {
    printf("\nInterrupt signal received. Exiting...\n");
    exit(EXIT_SUCCESS);
}
int main() {
    int pipefd[2];
    pid_t child[NUM_CHILD];
    char message[256];
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // Fork the children
    for (int i = 0; i < NUM_CHILD; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(pipefd[1]); // Close unused write end of pipe
            read(pipefd[0], message, sizeof(message));
            printf("Child %d: Message received: %s ", i, message);
            close(pipefd[0]); // Close read end of pipe
            exit(EXIT_SUCCESS);
        } else {
            child[i] = pid;
        }
    }
    // Parent process
    void sigint_handler(int a);
    if (signal(SIGINT, sigint_handler)== SIG_ERR);{
    	perror("signal");
    }
    printf("Enter a message to send to the children: ");
    fgets(message, sizeof(message), stdin);

    for (int i=0; i<NUM_CHILD; i++) {
   	 write(pipefd[1], message, sizeof(message));
   	 printf("Parent write to child %d.\n", i);
    }
    close(pipefd[0]); 

    for (int i = 0; i < NUM_CHILD; i++) {
        write(pipefd[1], message, sizeof(message));
    }
    close(pipefd[1]);

    for (int i = 0; i < NUM_CHILD; i++) {
        waitpid(child[i], NULL, 0);
    }
    printf(" Parent: All children has received messages. Exiting...\n ");

    return 0;
}
