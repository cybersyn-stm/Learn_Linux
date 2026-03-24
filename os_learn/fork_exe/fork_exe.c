#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// l 参数列表
// p 文件名，不指定路径则在PATH中查找
// v 参数数组形式传递
// e 使用新的环境变量
int main() {
    // pid1
    pid_t pid1 = fork();
    if (pid1 < 0) {
        printf("Fork Falied\n");
    } else if (pid1 == 0) {
        char *args[] = {"ls", NULL};
        printf("execvp():\n");
        fflush(stdout);
        execvp("ls", args);
        exit(1);
    }
    wait(NULL);
    // pid2
    pid_t pid2 = fork();
    if (pid2 < 0) {
        printf("Fork Falied\n");
    } else if (pid2 == 0) {
        printf("execl():\n");
        fflush(stdout);
        execl("/bin/ls", "ls", NULL);
        exit(1);
    }
    wait(NULL);
    // pid3
    pid_t pid3 = fork();
    if (pid3 < 0) {
        printf("Fork Falied\n");
    } else if (pid3 == 0) {
        printf("execle():\n");
        fflush(stdout);
        execle("/bin/ls", "ls", NULL, NULL);
        exit(1);
    }
    wait(NULL);
    // pid4
    pid_t pid4 = fork();
    if (pid4 < 0) {
        printf("Fork Falied\n");
    } else if (pid4 == 0) {
        printf("execlp():\n");
        execlp("/bin/ls", "ls", NULL);
        exit(1);
    }
    wait(NULL);
    // pid5
    pid_t pid5 = fork();
    if (pid5 < 0) {
        printf("Fork Falied\n");
    } else if (pid5 == 0) {
        printf("execv():\n");
        fflush(stdout);
        char *args[] = {"ls", NULL};
        execv("/bin/ls", args);
        exit(1);
    }
    wait(NULL);
    pid_t pid6 = fork();
    if (pid6 < 0) {
        printf("Fork Falied\n");
    } else if (pid6 == 0) {
        printf("execve():\n");
        fflush(stdout);
        char *args[] = {"ls", NULL};
        char *env[] = {"PATH=/bin", NULL};
        execve("/bin/ls", args, env);
        exit(1);
    }
    wait(NULL);
    printf("Child processes have finished. Parent process is exiting.\n");
    return EXIT_SUCCESS;
}
