#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char* indent(int tabs) {
    char* indentation = (char*) malloc(tabs + 1);

    for (int i = 0; i < tabs; ++i) {
        indentation[i] = '\t';
    }

    indentation[tabs] = '\0';
    return indentation;
}

const int N = 5;

int main() {
    for (int i = 0; i < N; ++i) {
        char* indentation = indent(i);

        int pid = fork();
        if (pid == 0) {
            printf("%s%d: parent with PID %d\n", indentation, i, getpid());
            wait(NULL); //
            printf("%s%d: parent with PID %d awaited child\n", indentation, i, getpid());
        } else {
            printf("%s%d: child with PID %d\n", indentation, i, getpid());
        }

        free(indentation);
    }
}
