#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 3

void handle_interrupt(int signal_num) {
    printf("Interrupt signal (%d) received. Exiting...\n", signal_num);
    exit(0);
}

int main() {
    int pipefd[2];
    pid_t children[NUM_CHILDREN];
    char buf[256];

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork the children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Child process
            close(pipefd[1]); // Close unused write end of pipe

            // Read message from pipe and display it
            read(pipefd[0], buf, sizeof(buf));
            printf("Child %d: Message received: %s\n", i, buf);

            close(pipefd[0]); // Close read end of pipe
            exit(0);
        } else {
            // Parent process
            children[i] = pid;
        }
    }

    // Parent process
    signal(SIGINT, handle_interrupt); // Handle interrupt signal

    printf("Enter a message to send to the children: ");
    fgets(buf, sizeof(buf), stdin);

    close(pipefd[0]); // Close unused read end of pipe

    // Send message to children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        write(pipefd[1], buf, sizeof(buf));
    }

    close(pipefd[1]); // Close write end of pipe

    // Wait for children to finish
    for (int i = 0; i < NUM_CHILDREN; i++) {
        waitpid(children[i], NULL, 0);
    }

    printf("Parent: All children have finished. Exiting...\n");

    return 0;
}
