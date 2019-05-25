#include <unistd.h> // getuid, geteuid
#include <sys/types.h> // uid_t
#include <stdio.h> // printf
#include <stdlib.h> // exit 

int main() {
    uid_t me = getuid();
    uid_t pretending = geteuid();

    printf("uid: %d euid: %d\n", me, pretending);
    exit(0);
}
