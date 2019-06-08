#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void close_descriptors(int first, int second, int third, int fourth) {
    close(first);
    close(second);
    close(third);
    close(fourth);
}

int main(const int argc, const char* argv[]) {
    if (argc != 5) {
        errx(1, "Invalid number of arguments. Expected 4 arguments for %s", argv[0]);
    }

    const int f1_dat_fd = open(argv[1], O_RDONLY);
    if (f1_dat_fd == -1) {
        err(1, "Could not open %s", argv[1]);
    }

    const int f1_idx_fd = open(argv[2], O_RDONLY);
    if (f1_idx_fd == -1) {
        const int saved_errno = errno;
        close(f1_dat_fd);
        errno = saved_errno;
        err(1, "Could not open %s", argv[2]);
    }
    
    const int f2_dat_fd = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (f2_dat_fd == -1) {
        const int saved_errno = errno;
        close(f1_dat_fd);
        close(f1_idx_fd);
        errno = saved_errno;
        err(1, "Could not open %s", argv[3]);
    }

    const int f2_idx_fd = open(argv[4], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (f2_idx_fd == -1) {
        const int saved_errno = errno;
        close(f1_dat_fd);
        close(f1_idx_fd);
        close(f2_dat_fd);
        errno = saved_errno;
        err(1, "Could not open %s", argv[4]);
    }

    typedef struct {
        uint16_t offset;
        uint8_t length;
        uint8_t saved;
    } __attribute__((packed)) entry_t;

    entry_t new = { .offset = 0, .length = 0, .saved = 0 };

    while (true) {
        entry_t old;
        if (read(f1_idx_fd, &old, sizeof(old)) != sizeof(old)) {
            break;
        }

        if (old.length < 1) {
            continue;
        }

        if (lseek(f1_dat_fd, old.offset, SEEK_SET) == -1) {
            const int saved_errno = errno;
            close_descriptors(f1_dat_fd, f1_idx_fd, f2_dat_fd, f2_idx_fd);
            err(1, "Could not seek to %04x in %s", old.offset, argv[1]);
            errno = saved_errno;
        }

        printf("offset = %04x length = %02x \n", old.offset, old.length);
        char buffer;
        bool skip = false;

        for (int idx = 0; idx < old.length; ++idx) {
            if (read(f1_dat_fd, &buffer, sizeof(buffer)) != sizeof(buffer)) {
                const int saved_errno = errno;
                close_descriptors(f1_dat_fd, f1_idx_fd, f2_dat_fd, f2_idx_fd);
                errno = saved_errno;
                err(1, "Could not read byte of set [%04x : %02x] from file %s",
                        old.offset, old.length, argv[1]);
            }

            printf("idx= %d char = %c \n", idx, buffer);
            if ((idx == 0) && ((buffer < 'A') || (buffer > 'Z'))) {
                skip = true;
                break;
            }

            if (write(f2_dat_fd, &buffer, sizeof(buffer)) != sizeof(buffer)) {
                const int saved_errno = errno;
                close_descriptors(f1_dat_fd, f1_idx_fd, f2_dat_fd, f2_idx_fd);
                errno = saved_errno;
                err(1, "Could no write by of set [%04x : %02x] to file %s",
                        old.offset, old.length, argv[3]);
            }
        }

        if (skip) {
            continue;
        }

        new.offset += new.length;
        new.length = old.length;

        if (write(f2_idx_fd, &new, sizeof(new)) != sizeof(new)) {
            const int saved_errno = errno;
            close_descriptors(f1_dat_fd, f1_idx_fd, f2_dat_fd, f2_idx_fd);
            errno = saved_errno;
            err(1, "Could not add new set description to file %s, %s contains non-indexed data!",
                    argv[4], argv[3]);
        }
    }

    close_descriptors(f1_dat_fd, f1_idx_fd, f2_dat_fd, f2_idx_fd);

    exit(0);
}
