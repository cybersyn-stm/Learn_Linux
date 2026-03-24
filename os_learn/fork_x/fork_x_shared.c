#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int *x = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (x == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    *x = 100;
    printf("shared init x = %d\n", *x);

    pid_t ret = fork();
    wait(NULL);
    *x = *x + 100;
    if (ret < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (ret == 0) {
        /* child: increment before parent waits */
        printf("I'm child process, the x = %d\n", *x);
        return EXIT_SUCCESS;
    } else {
        /* parent: wait for child, then increment */
        printf("I'm parent process, the x = %d\n", *x);
    }

    munmap(x, sizeof(int));
    return EXIT_SUCCESS;
}
