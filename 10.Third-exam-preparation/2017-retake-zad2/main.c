#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXPAR 2
#define MAXLEN 4

typedef char par_array[MAXPAR][MAXLEN + 1];

static void call(const char* const command, par_array params, const size_t n) {
    const pid_t child_pid = fork();
    if (child_pid == -1) {
        err(1, "cannot fork");
    }

    if (child_pid == 0) {
        if (n == 2) {
            if (execlp(command, command, params[0], params[1], NULL) == -1) {
                err(1, "cannot exec");
            }
        } else if (n == 1) {
            if (execlp(command, command, params[0], NULL) == -1) {
                err(1, "cannot exec");
            }
        }
    }

    int wait_status;
    waitpid(child_pid, &wait_status, 0);
}

int main(const int argc, const char* argv[]) {
    if (argc > 2) {
        errx(1, "Usage %s [optional-command-name]\n", argv[0]);
    }

    if ((argc == 2) && (strlen(argv[1]) > MAXLEN)) {
        errx(1, "Max command-name length: %d", MAXLEN);
    }

    const char* default_cmd = "echo";
    const char* cmd = (argc == 2) ? argv[1] : default_cmd;

    par_array par;

    char buffer;

    ssize_t rs = 0;
    size_t curr_par = 0;
    size_t curr_pos = 0;

    bool ready = false;

    while ( (rs = read(0, &buffer, sizeof(buffer))) == sizeof(buffer) ) {
        if ((buffer == ' ') || (buffer == '\n')) {
            par[curr_par][curr_pos] = '\0';

            if (curr_pos == 0 && curr_par == 0) {
                continue;
            } else {
                if (curr_par == 0) {
                    curr_par = 1;
                    curr_pos = 0;
                } else {
                    curr_par = 0;
                    curr_pos = 0;
                    ready = true;
                }
            }
        } else {
            if (curr_pos == MAXLEN) {
                errx(1, "input too long");
            }

            par[curr_par][curr_pos++] = buffer;
        }

        if (ready) {
            ready = false;
            call(cmd, par, MAXPAR);
        }
    }

    par[curr_par][curr_pos] = '\0';

    if (curr_par != 0 || curr_pos != 0) {
        size_t count = curr_pos != 0 ? curr_par + 1 : curr_par;
        call(cmd, par, count);
    }

    return 0;
}
