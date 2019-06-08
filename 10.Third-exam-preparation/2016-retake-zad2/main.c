#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#define BUFFER_SIZE 4096

int main(const int argc, const char* argv[]) {

    if (argc != 4) {
        errx(1, "Invalid number of arguments, %s. Expected 3 arguments", argv[0]);
    }

    int patch_fd = open(argv[1], O_RDONLY);
    if (patch_fd == -1) {
        err(2, "Error opening %s", argv[1]);
    }

    int f1_fd = open(argv[2], O_RDONLY);
    if (f1_fd == -1) {
        int saved_errno = errno;
        close(patch_fd);
        errno = saved_errno;
        err(2, "Error opening %s", argv[2]);
    }

    int f2_fd = open(argv[3], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (f2_fd == -1) {
        int saved_errno = errno;
        close(patch_fd);
        close(f1_fd);
        errno =saved_errno;
        err(2, "Error opening %s", argv[3]);
    }

    uint8_t buffer[BUFFER_SIZE];
    ssize_t read_size = 0;

    while ((read_size = read(f1_fd, buffer, sizeof(buffer))) > 0) {
        if (write(f2_fd, buffer, read_size) != read_size) {
            err(1, "Error writing to %s", argv[2]);
        }
    }

    close(f1_fd);
    lseek(f2_fd, 0, SEEK_SET);

    struct {
        uint16_t offset;
        uint8_t orgbyte;
        uint8_t newbyte;
    } element;

    uint8_t byte = 0;
    while (read(patch_fd, &element, sizeof(element)) == sizeof(element)) {
        printf("%04x %02x %02x \n", element.offset, element.orgbyte, element.newbyte);
        
        if (lseek(f2_fd, element.offset, SEEK_SET) < 0) {
            int saved_errno = errno;
            close(f2_fd);
            errno = saved_errno;
            err(1, "llseek in %s to %04x failed", argv[2], element.offset);
        }

        if (read(f2_fd, &byte, 1) != 1) {
            int saved_errno = errno;
            close(f2_fd);
            errno = saved_errno;
            err(1, "read from %s at %04x failed!",
                    argv[2], element.offset);
        }

        if (byte != element.orgbyte) {
            int saved_errno = errno;
            close(f2_fd);
            errno = saved_errno;
            err(1, "byte in %s at %04x is different from %02x", 
                    argv[2], element.offset, element.orgbyte);
        }           

        lseek(f2_fd, -1, SEEK_CUR);

        if (write(f2_fd, &element.newbyte, 1) != 1) {
            int saved_errno = errno;
            close(f2_fd);
            errno = saved_errno;
            err(1, "write in %s at %04x of %02x failed",
                    argv[2], element.offset, element.newbyte);
        }
    }

    close(f2_fd);
    close(patch_fd);

    exit(0);
}
