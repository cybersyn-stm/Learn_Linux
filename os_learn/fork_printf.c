/* fork_printf.c
 * Demonstrate fork() and closing STDOUT in the child process.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Child: close standard output and try to write to it */
        close(STDOUT_FILENO);
        /* These attempts to print to stdout should fail / not appear */
        puts("child: puts to stdout (should NOT appear)");
        fflush(stdout);
        ssize_t w = write(STDOUT_FILENO, "child: write to stdout\n", 23);
        if (w < 0) {
            fprintf(stderr, "child: write failed: %s\n", strerror(errno));
        }
        _exit(0);
    } else {
        /* Parent: stdout still open */
        printf("parent (pid=%d): stdout is open\n", getpid());
        wait(NULL);
    }

    return 0;
}
