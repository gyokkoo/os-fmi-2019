/*
 * Примерно използване на stat
 */

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int main() {
    struct stat sb; // Stat buffer

    stat("Makefile", &sb);

    printf("last accessed: %s", ctime(&sb.st_atime));
    printf("lat modified:  %s", ctime(&sb.st_mtime));
    printf("last change:   %s", ctime(&sb.st_ctime));

    return 0;
}
