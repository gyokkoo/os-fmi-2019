#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(const int argc, const char* argv[]) {
    if (argc == 1) {
        errx(1, "Invalid program arguments!");
    }

    int fd = open("serror.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    dup2(fd, 2);

    if (execvp("/usr/bin/sort", argv) == -1) {
        errx(2, "error with date command");
    }
}
