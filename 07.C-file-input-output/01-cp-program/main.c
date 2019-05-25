/*
 * Реализирайте команда cp, работеща с два аргумента, подадени като входни параметри.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#define BUFFER_SIZE 16384

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "Incorrect number of arguments!");
    }

    int fd_read = open(argv[1], O_RDONLY);
    if (fd_read == -1) {
        errx(2, "Could not open first argument!");
    }

    int fd_write = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0644);
    if (fd_write == -1) {
        close(fd_read);
        errx(2, "Could not open second argument!");
    }

    char buffer[BUFFER_SIZE];
    int read_return_value = -1;
    int write_return_value = -1;
    while ( (read_return_value = read(fd_read, &buffer, 1) ) != 0) {
        if (read_return_value == -1) {
            close(fd_read);
            close(fd_write);
            errx(3, "Reading file failed!");
        }

        // If successful read() returns the number of bytes that are read
        write_return_value = write(fd_write, &buffer, read_return_value);
        if (write_return_value != 1) {
            close(fd_read);
            close(fd_write);
            errx(3, "Writing to file failed!");
        }
    }

    close(fd_read);
    close(fd_write);
    exit(0);
}
