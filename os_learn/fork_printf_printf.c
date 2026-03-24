/* fork_printf_printf.c
 * Demonstrate behavior of printf/fflush/write/puts after closing STDOUT_FILENO in a child process.
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
        /* Child: close stdout and attempt various output methods */
        fprintf(stderr, "child(%d): closing STDOUT_FILENO\n", getpid());
        if (close(STDOUT_FILENO) < 0) {
            fprintf(stderr, "child: close failed: %s\n", strerror(errno));
        }

        /* printf (high-level stdio) */
        errno = 0;
        int r = printf("child: printf to stdout (should NOT appear)\n");
        if (r < 0) {
            fprintf(stderr, "child: printf returned %d, errno=%d (%s)\n", r, errno, strerror(errno));
        } else {
            fprintf(stderr, "child: printf returned %d (buffered)\n", r);
        }

        /* fflush to force the buffered data through the closed fd */
        errno = 0;
        if (fflush(stdout) == EOF) {
            int e = errno;
            fprintf(stderr, "child: fflush failed, errno=%d (%s)\n", e, strerror(e));
        } else {
            fprintf(stderr, "child: fflush succeeded\n");
        }

        /* low-level write on the closed descriptor */
        errno = 0;
        const char *msg = "child: write to stdout (low-level)\n";
        ssize_t w = write(STDOUT_FILENO, msg, strlen(msg));
        if (w < 0) {
            int e = errno;
            fprintf(stderr, "child: write returned %zd, errno=%d (%s)\n", w, e, strerror(e));
        } else {
            fprintf(stderr, "child: write succeeded, wrote %zd bytes\n", w);
        }

        /* puts as another stdio call */
        errno = 0;
        int p = puts("child: puts to stdout (should NOT appear)");
        if (p == EOF) {
            int e = errno;
            fprintf(stderr, "child: puts returned EOF, errno=%d (%s)\n", e, strerror(e));
        } else {
            fprintf(stderr, "child: puts returned %d\n", p);
        }

        /* Final note and exit */
        fprintf(stderr, "child(%d): exiting\n", getpid());
        _exit(0);
    } else {
        /* Parent: stdout still open */
        printf("parent(%d): stdout is open\n", getpid());
        wait(NULL);
        printf("parent(%d): child finished\n", getpid());
    }

    return 0;
}
