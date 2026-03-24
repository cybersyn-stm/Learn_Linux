#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int x;
    x = 100;
    printf("init x = %d\n", x);
    int ret = fork();
    x = x + 100;
    if (ret == 0) {
        printf("I'm child process, the x = %d\n", x);
    } else {
        wait(NULL);
        printf("I'm parent process, the x = %d\n", x);
    }

    return EXIT_SUCCESS;
}
