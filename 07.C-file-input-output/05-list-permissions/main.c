/* Примерно използване на stat */

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>

int main(int argc, char* argv[]) {
    struct stat sb;

    char *filetype[] = { "unknown", "FIFO", "character device", "unknown", "directory",
                         "unknown", "block device", "unknown", "file",
                         "unknown", "symlink", "unknown", "socket" };

    if (argc != 2) {
        errx(1, "Error! This program accepts only one filename!");   
    }

    if (stat(argv[1], &sb) < 0) {
        perror(argv[1]);
        exit(2);
    }

    printf("file type: %s\n", filetype[(sb.st_mode >> 12) & 017]);
    printf("permissions:   %c%c%c%c%c%c%c%c%c\n",
         (sb.st_mode & S_IRUSR) ? 'r' : '-',
         (sb.st_mode & S_IWUSR) ? 'w' : '-',
         (sb.st_mode & S_IXUSR) ? 'x' : '-',
         (sb.st_mode & S_IRGRP) ? 'r' : '-',
         (sb.st_mode & S_IWGRP) ? 'w' : '-',
         (sb.st_mode & S_IXGRP) ? 'x' : '-',
         (sb.st_mode & S_IROTH) ? 'r' : '-',
         (sb.st_mode & S_IWOTH) ? 'w' : '-',
         (sb.st_mode & S_IXOTH) ? 'x' : '-' );

    exit(0);
}
