/*
 * Напишете програма на С, коята по подадено име на (текстови) файл като параметър, извежда съдържанието на файла сортирано, 
 * чреа употреба на външните програми cat и sort през pipe()
 */

#include <stdlib.h> 
#include <sys/types.h> // pid_t
#include <err.h> 
#include <unistd.h> // pipe

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "Invalid number of arguments!");
    }

    int fd[2];
    pipe(fd);

    pid_t cat_pid;
    if ( ((cat_pid) = fork()) == -1 ) {
        err(1, "cannot fork cat");
    }

    if (cat_pid == 0) {
        //child
        close(fd[0]);

        if (dup2(fd[1], 1) == -1) {
            err(1, "dup2 cat failed");
        }

        if (execlp("cat", "cat", argv[1], NULL) == -1) {
            err(1, "exec cat failed");       
        }
    }

    close(fd[1]);

    if (dup2(fd[0], 0) == -1) {
        err(1, "dup2 parent");
    }

    if (execlp("sort", "sort", "-", NULL) == -1) {
        err(1, "exec sort");
    }

    exit(0);
}
