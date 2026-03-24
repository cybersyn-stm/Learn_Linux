#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    printf("Hello from the parent process! My PID is %d\n", getpid());
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed!\n");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // This is the child process
        printf("Hello from the child process! My PID is %d\n", getpid());
        printf("I will use wait()\n");
        int wait_child = wait(NULL); // Wait for any child process to finish
        printf("Child wait return : %d.\n", wait_child);
        if (wait_child < 0) {
            printf("Wait failed in child process!\n");
        }
        sleep(2); // Simulate some work in the child process
        printf("Child process is exiting.\n");
        return EXIT_SUCCESS;
    } else {
        // This is the parent process
        printf("Parent process is waiting for the child to finish...\n");
        int wait_return = wait(NULL); // Wait for the child process to finish
        printf("Wait return: %d.\n", wait_return);
        printf("Child process with PID has finished.\n");
    }
    return EXIT_SUCCESS;
}
