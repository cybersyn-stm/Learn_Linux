#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*1. 正常退出（exited）
   - 用 WIFEXITED(status) 判断，返回真表示子进程正常退出。
   - 用 WEXITSTATUS(status) 获取退出码。

2. 被信号终止（signaled）
   - 用 WIFSIGNALED(status) 判断，返回真表示子进程被信号终止。
   - 用 WTERMSIG(status) 获取导致终止的信号编号。

3. 停止（stopped）
   - 用 WIFSTOPPED(status) 判断，返回真表示子进程被信号停止（如 Ctrl+Z）。
   - 用 WSTOPSIG(status) 获取导致停止的信号编号。

4. 恢复运行（continued）
   - 用 WIFCONTINUED(status) 判断，返回真表示子进程收到 SIGCONT 信号后继续运行（需 WCONTINUED 选项）。*/

/*- 0：默认，阻塞等待子进程结束。
- WNOHANG：非阻塞，如果没有子进程退出，立即返回 0。
- WUNTRACED：如果子进程因信号停止（但未终止），也返回其状态。
- WCONTINUED：如果子进程因 SIGCONT 信号恢复运行，也返回其状态（需支持）。*/
int main() {
    printf("Use waitpid()\n");
    pid_t pid1 = fork();
    if (pid1 < 0) {
        printf("fork faild\n");
        return 0;
    }
    if (pid1 == 0) {
        printf("Child process, pid:%d\n", getpid());
        exit(0);
    } else {
        int status;
        waitpid(pid1, &status, 0);
        printf("Parent process, pid:%d\n", getpid());
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process did not exit normally\n");
        }
    }
    return 0;
}
