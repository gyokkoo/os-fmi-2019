#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pid = fork();
    if (pid > 0) {
        /* Parent */
        printf("Parent waiting for child ...\n");
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
             printf("child ended normally, exit status = %d\n", WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            printf("child terminated by signal %d\n", WTERMSIG(status));
        }
    } else if (pid == 0) {
        /* Child */
        printf("child running -- PID is %d\n", getpid());
        // Terminate the program on purpose with signal 11 (Segmantation fault)
        // *(int *) 0 = 24
        sleep(5);
        exit(1);
    }
}
