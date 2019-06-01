/*
 * Напишете програма, която приема точно 2 аргумента. 
 * Първият може да бъде само --min, --max или --print (вижте man 3 strcmp). 
 * Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа (uint16_t - вижте man stdint.h). 
 * Ако първият аргумент е:
 * --min - програмата отпечатва кое е най-малкото число в двоичния файл.
 * --max - програмата отпечатва кое е най-голямото число в двоичния файл.
 * --print - програмата отпечатва на нов ред всяко число.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        errx(1, "Invalid number of arguments!");
    }

    int fd_read = open(argv[2], O_RDONLY);
    if (fd_read == -1) {
        errx(2, "Could not open second argument file");
    }
    
    uint16_t num; // 2 bytes int buffer
    uint16_t max = 0;
    uint16_t min = 65535;
    ssize_t read_size = 0;
    do {
        read_size = read(fd_read, &num, sizeof(num));
        if (read_size < 0) {
            err(1, "Error reading %s", argv[2]);
        }
        
        if (num > max) {
            max = num;
        } else if (num < min) {
            min = num;
        }

        if (strcmp(argv[1], "--print") == 0) {
           printf("uint16_t = %d \n", num); 
        }
    } while (read_size > 0);

    if (strcmp(argv[1], "--min") == 0) {
        printf("min = %d \n", min);
    } else if (strcmp(argv[1], "--max") == 0) {
        printf("max = %d \n", max);
    }

    exit(0);
}
