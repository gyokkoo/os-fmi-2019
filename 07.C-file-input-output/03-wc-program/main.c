/*
 * Реализирайте команда wc, с един аргумент подаден като входен параметър
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid count of arguments!\n");
        exit(1);
    }
 
    int fd_read = open(argv[1], O_RDONLY);
    if (fd_read == -1) {
        fprintf(stderr, "Cannot open first argument!\n");
        exit(1);
    }

    int words = 0;
    int lines = 0;
    int chars = 0;
    char symbol;
    while (read(fd_read, &symbol, 1) > 0) {
        if (symbol == '\n') {
            lines++;
            words++;
        }

        if (symbol == ' ') {
            words++;
        }

        chars++;
    }

    printf("File %s has:\n%d number of lines.\n%d number of words.\n%d number of chars.\n", argv[1], lines, words, chars);
}
