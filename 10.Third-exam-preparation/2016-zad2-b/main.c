#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    int waitStatus;
    char cmd[32];
    int i = 0;

    while (1) {
        write(1, "> ", 2);

        while ( (read(0, &cmd[i], 1)) && cmd[i] != '\n' ) {
            if (cmd[i] == ' ' || cmd[i] == '\t') {
                continue;
            }

            ++i;
        }

        if (cmd[i] == '\n') {
            cmd[i] = '\0';
        }

        if (strcmp(cmd, "exit") == 0) {
            exit(0);
        }

        if (fork() > 0) {
            wait(&waitStatus);
            i = 0;
        } else {
            printf("%s", cmd);
            if (execlp(cmd, cmd, NULL) < 0) {
                printf("cannot execute %s \n", cmd);
                exit(1);
            }
        }
    }
}
