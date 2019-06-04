#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* A very minimal shell*/
int main() {
    char line[100];

    /* Main command loop */
    while (printf("> "), gets(line) != NULL) {
        if (fork() == 0) { /* Child */
            /* The first  argument to execlp  is the name of
               the  executable,  which will  be looked up on
               the search path (defined by PATH)

               The remaining arguments are  the command line
               args that will be passed to the program, with
               a terminating NULL
            */
	        execlp(line, line, (char *)0);

            /* Don't come here unless execlp fails */
	        printf("%s: not found\n", line);
	        exit(1);
	    }
	    else {
            /* Parent */
            wait(0);
        }
    }
}
