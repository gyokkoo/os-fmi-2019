/*
 * Small shell with pipes, illustrating
 * fork(), exec(), wait(), pipe(), dup2(), close()
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* 
 * This function prints a prompt, reads in a command
 * line,  and  parses it,  placing  pointers  to the
 * individual  tokens prior to a "|"  token into the
 * upstream  array  (u),  and tokens following a "|"
 * token (if any)  into  the  downstream  array (d).
 * Both arrays are terminated by a NULL pointer
*/

int prompt_and_parse(char** u, char** d) {
    static char line[100];
    d[0] = NULL;
    printf("> ");
    if (gets(line) == NULL) {
        return -1;
    }

    *u++ = strtok(line, " \t");
    
    while ((*u = strtok(NULL, " \t"))) {
        if (strcmp(*u, "|") == 0) {
            *u = NULL;
            while ((*d++ = strtok(NULL, " \t"))) {
            }

            return 1;
        }

        u++;
    }
    return 1;
}

int main() {
    int p[2];
    char* upstream[20];
    char* downstream[20];

    while (prompt_and_parse(upstream, downstream) > 0) {
        // If downstream[0] is NULL, there is no pipeline
        if (downstream[0] == NULL) {
            if (fork() == 0) {
                execvp(upstream[0], upstream);
                printf("%s: not found \n", upstream[0]);
                exit(1);
            } else {
                wait(0);
            }
        } else {
            // There is a pipeline, so must fork
            pipe(p);
            if (fork() == 0) {
                // Connect stdout to upstream end of pipe
                dup2(p[1], 1);

                // Close downstream end of pipe
                close(p[0]);

                // Execute the upstream command
                execvp(upstream[0], upstream);
                printf("%s: not found\n", upstream[0]);
                exit(1);
            }
            if (fork() == 0) { // Downstream
                // Connect stdin to downstream end of pipe
                dup2(p[0], 0);
                // Close upstream end of pipe
                close(p[1]);

                execvp(downstream[0], downstream);
                printf("%s: not found\n", downstream[0]);
                exit(1);
            } else { // Parent
                // Close both ends of pipe
                close(p[0]);
                close(p[1]);

                // Wait for both children to finish
                wait(0);
                wait(0);
            }
        }
    }
}
