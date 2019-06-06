#include <unistd.h>

// Equivalent to "ls | sort -r"
int main() {
    int p[2];
    
    pipe(p); // Create the pipe

    int pid = fork();
    if (pid == 0) {
        // Child: connect stdout to pipe
        dup2(p[1], 1);
        close(p[0]);
        execlp("ls", "ls", NULL);
    }

    // Parent: connect stdin to pipe
    dup2(p[0], 0);
    close(p[1]);
    execlp("sort", "sort", "-r", NULL);
}
