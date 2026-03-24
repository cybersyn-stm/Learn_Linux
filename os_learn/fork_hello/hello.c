#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    int ret = fork();
    if (ret == 0) {
        printf("hello\n");
    }
    if (ret > 0) {
        usleep(1);
        printf("goodbye\n");
    }
    return EXIT_SUCCESS;
}
