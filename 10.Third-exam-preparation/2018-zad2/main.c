#include <unistd.h>
#include <err.h>
#include <sys/types.h>

// find | sort | awk | head
int main(const int argc, const char* argv[]) {

    if (argc != 2) {
        errx(1, "Invalid arguments count!");
    }

    int fd1[2];
    int fd2[2];
    int fd3[2];

    pipe(fd1);
    pipe(fd2);
    pipe(fd3);

    pid_t find_pid = fork();
    if (find_pid == -1) {
        err(1, "fork find failed");
    }

    if (find_pid == 0) {
        close (fd1[0]);

        if (dup2(fd1[1], 1) == -1) {
            err(1, "dup2 find failed");
        }

        if (execlp("find", "find", argv[1], "-type", "f", "-ls", NULL) == -1) {
            err(1, "exec cut failed!");
        }
    }

    close(fd1[1]);

    pid_t sort_pid = fork();
    if (sort_pid == -1) {
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

        if (execlp("sort", "sort", "-M", "-k8,10", NULL) == -1) {
            err(1, "exec sort failed");
        }
    }

    close(fd2[1]);

    pid_t awk_pid = fork();
    if (awk_pid == -1) {
        err(1, "fork awk failed");
    }

    if (awk_pid == 0) {
        close(fd3[0]);

        if (dup2(fd2[0], 0) == -1) {
            err(1, "dup2 0 awk failed");
        }

        if (dup2(fd3[1], 1) == -1) {
            err(1, "dup2 1 awk failed");
        }

        if (execlp("awk", "awk", "{print $11}", NULL) == -1) {
            err(1, "exec awk failed");
        }
    }

    close(fd3[1]);

    if (dup2(fd3[0], 0) == -1) {
        err(1, "dup2 0 head failed");
    }

    if (execlp("head", "haed", "-n1", NULL) == -1) {
        err(1, "exec head failed");
    }
}
