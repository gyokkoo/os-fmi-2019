#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <err.h>

int main() {
    int f1_fd = open("f1", O_RDONLY);

    if (f1_fd < 0) {
        err(1, "Cannot open file f1!");
    }

    int f2_fd = open("f2", O_RDONLY);
    if (f2_fd < 0) {
        err(1, "Cannot open file f2!");
    }

    int f3_fd = open("f3", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (f3_fd < 0) {
        err(1, "Cannot open file f3!");
    }

    uint32_t buff[2];
    uint32_t num;
    while (read(f1_fd, buff, sizeof(buff)) == sizeof(buff)) {
        off_t lpt;
        lpt = lseek(f2_fd, buff[0] * sizeof(num), SEEK_SET);
        if (lpt < 0) {
            err(1, "lseek error on file f1");
        }

        for (uint32_t i = 0; i < buff[1]; ++i) {
            read(f2_fd, &num, sizeof(num));
            write(f3_fd, &num, sizeof(num));
        }
    }

    close(f1_fd);
    close(f2_fd);
    close(f3_fd);
}
