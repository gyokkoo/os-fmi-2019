/*
 * Реализирайте команда head без опции 
 * (т.е. винаги да извежда на стандартния изход само 
 * първите 10 реда от съдържанието на файл подаден като първи параматър)
 */

#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int fd_read;
    int counter = 0;
    char symbol;

    if (argc != 2) {
        errx(-1, "Invalid count of arguments!");
    }

    fd_read = open(argv[1], O_RDONLY);
    if (fd_read == -1) {
       errx(-1, "Could not open file to read!");
    } 

    while (read(fd_read, &symbol, 1)) {
        if (symbol == '\n') {
            counter = counter + 1;
        }

        write(1, &symbol, 1);
        if (counter == 10) {
            break;
        }
    }

    close(fd_read);
    exit(0);
}
