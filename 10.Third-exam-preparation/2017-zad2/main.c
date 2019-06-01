#include <unistd.h>
#include <err.h>
#include <sys/types.h>

int main() {
    int fd1[2];
    int fd2[2];
    int fd3[2];

    pipe(fd1);
    pipe(fd2);
    pipe(fd3);

    pid_t cut_pid;
    if ((cut_pid = fork()) == -1) {
        err(1, "fork cut failed");
    }

    if (cut_pid == 0) {
        close(fd1[0]);

        if (dup2(fd1[1], 1) == -1) {
            err(1, "dup2 cut failed");
        }

        if (execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", NULL) == -1) {
            err(1, "exec cut failed");
        }
    }

    close(fd1[1]);

    pid_t sort_pid;
    if ((sort_pid = fork()) == -1) {
        err(1, "fork sort failed");
    }

    if (sort_pid == 0) {
        close(fd2[0]);

        if (dup2(fd1[0], 0) == -1) {
            err(1, "dup2 0 sort failed");
        }

        if (dup2(fd2[1], 1) == -1) {
            err(1, "dup2 1 sort failed");
        }

        if (execlp("sort", "sort", "-", NULL) == 1) {
            err(1, "exec sort failed");
        }
    }

    close(fd2[1]);
   
    pid_t uniq_pid; 
    if ((uniq_pid = fork()) == -1) {
        err(1, "fork uniq failed");
    }

    if (uniq_pid == 0) {
        close(fd3[0]);

        if (dup2(fd2[0], 0) == -1) {
            err(1, "dup2 2 uniq failed");
        }

        if (dup2(fd3[1], 1) == -1) {
            err(1, "dup 3 uniq failed");
        }

        if (execlp("uniq", "uniq", "-c", NULL)) {
            err(1, "exec uniq -c failed");
        }
    }

    close(fd3[1]);
    
    if (dup2(fd3[0], 0) == -1) {
        err(1, "dup2 3 sort -n failed");
    }

    execlp("sort", "sort", "-n", NULL);

    err(1, "exec sort -n");
}
