/* Напишете програма на C, която приема параметър - име на (двоичен) файл с байтове. 
 * Програмата трябва да сортира файла 
 */

#include <err.h>
#include <fcntl.h>
#include <stdint.h> // uint8_t
#include <stdio.h>
#include <unistd.h> // lseek
#include <errno.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "Error! Expected one argument");
    }
    
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        errx(1, "Error opening argument file");
    }

    const size_t BUFFER_SIZE = 4096;
    uint8_t buffer[BUFFER_SIZE];

    ssize_t read_size = 0;

    unsigned int stats[256] = { 0 }; 

    do {
        read_size = read(fd, &buffer, sizeof(buffer));
        if (read_size < 0) {
            int saved_errno = errno;
            close(fd);
            errno = saved_errno;
            err(1, "%s", argv[1]);
        }

        for (unsigned int i = 0; i < read_size; i++) {
                stats[buffer[i]]++;
        }
    }
    while (read_size > 0);

    lseek(fd, 0, SEEK_SET);

    for (int i = 0; i < 256; i++) {
        uint8_t c = i;
        while (stats[i] > 0) {
            if (write(fd, &c, 1) != 1) {
                int saved_errno = errno;
                close(fd);
                errno = saved_errno;
                err(1, "Error while writing");
            }

            stats[i]--;
        }
    }

    close(fd);

    exit(0);
}
